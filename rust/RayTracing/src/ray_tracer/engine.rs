use nalgebra_glm as glm;
use rand::{prelude::ThreadRng};
use rayon::prelude::*;
use hittable::primitives::primitive::Primitive;
use crate::utils::rnd_random_vec;
use super::{camera::Camera, ray::Ray, hittable::{self, Hittable}};
use std::{sync::{Arc, Mutex, Condvar, mpsc}, time::Duration};
use std::thread;

fn get_index(i: usize, img_size: &(usize, usize)) -> [f32; 2]
{
    [(i%img_size.0) as f32, (i/img_size.0) as f32]
}

fn gen_uv(xy: &glm::Vec2, img_size: &(usize, usize)) -> glm::Vec2
{
    let normalized: glm::Vec2 = [xy.x/(img_size.0 as f32 - 1.0), xy.y/(img_size.1 as f32 - 1.0)].into();

    (normalized.add_scalar(-0.5f32)).component_mul(&glm::vec2(2.0f32, 2.0))
}


fn sky(ray: &Ray) -> glm::Vec3
{
    glm::mix(&[1.0f32, 1.0, 1.0].into(), &[0.5f32, 0.7, 1.0].into(), (ray.dir.y+1.0)*0.5)
}

fn shade(ray: &Ray, world: &Primitive, max_bouces: usize, rng: &mut ThreadRng) -> glm::Vec3
{
    if max_bouces == 0
    {
        return glm::vec3(0.0, 0.0, 0.0);
    }

    if let Some(hit) = world.intersect(ray, 0.001, f32::INFINITY)
    {
        if let (Some(out), color) = hit.mat.scatter(ray, &hit, rng)
        {
            shade(&out, world, max_bouces-1, rng).component_mul(&color)
        }
        else 
        {
            glm::vec3(0.0, 0.0, 0.0)
        }
    }
    else 
    {
        sky(&ray)
    }
}


pub struct RayTracer
{
    request: Arc<(Mutex<State>, Condvar)>,
    data: Arc<Mutex<(Vec<glm::Vec3>, i32)>>,
    render_settings: mpsc::Sender<RenderEnv>,
    telemetry: Arc<Mutex<(i32, Duration)>>,
    max_samples: i32,
}

#[derive(Default)]
pub struct RenderEnv
{
    pub world: Option<Primitive>,
    pub camera: Option<Camera>,
}

#[derive(PartialEq, Debug)]
enum State {
    RENDERING, // Render thread is writing to buffer 
    REQUEST,   // Window thread wants to read
    READING,   // Render thread waits for window thread
}

impl RayTracer
{
    pub fn new(surface: Vec<glm::Vec3>, camera: Camera, world: Primitive, img_size: (usize, usize), max_samples: usize, bouces: usize) -> Self
    {
        let data_request = Arc::new((Mutex::new(State::RENDERING), Condvar::new()));
        let data = Arc::new(Mutex::new((surface, 0)));
        let telemetry = Arc::new(Mutex::new((0, Duration::new(0,0))));
        let (tx, rx) = mpsc::channel::<RenderEnv>();

        let tracer = RayTracer{ request: data_request.clone(), data: data.clone(), render_settings: tx, telemetry: telemetry.clone(), max_samples: max_samples as i32};

        thread::spawn(
            move ||
            {      
                let (worker_request, conv) = &*data_request;
                let data = &*data;
                let telemetry = &*telemetry;

                let mut world = world;
                let mut camera = camera;

                loop 
                {
                    {                        
                        let mut guard = worker_request.lock().unwrap();
                        
                        if *guard == State::REQUEST
                        {
                            *guard = State::READING;

                            drop(guard);

                            conv.notify_all();
                            let _ = conv.wait_while(worker_request.lock().unwrap(), |x| { *x != State::RENDERING }).unwrap();
                        }
                    }

                    {
                        let mut locked = data.lock().unwrap();
                        
                        for new_env in rx.try_recv()
                        {
                            if new_env.camera.is_some() || new_env.world.is_some()
                            {
                                locked.1 = 0;
                                locked.0.par_iter_mut().for_each(|x| *x=[0.0,0.0,0.0].into());
                            }

                            if let Some(_world) = new_env.world
                            {
                                world = _world;
                            }
                            
                            if let Some(_camera) = new_env.camera
                            {
                                camera = _camera;
                            }
                        }

                        if locked.1 < max_samples as i32
                        {
                            let start = std::time::Instant::now();
                            trace(&mut locked.0, &camera, &world, img_size, 1 as i32, bouces);
                            let end = std::time::Instant::now();
                            locked.1 += 1;
                            
                            {
                                let mut guard = telemetry.lock().unwrap();
                                let time = end-start;
                                
                                guard.0 = locked.1;
                                guard.1 += time;
                            }
                        }
                    } 
                }
            }
        );

        return tracer;
    }

    pub fn request_data<F: FnOnce(&Vec<glm::Vec3>, &i32)>(&mut self, f: F)
    {
        {
            let (mutex, conv) = &*self.request;
            let data = &*self.data;
            
            let (mut guard, _) = conv.wait_timeout(mutex.lock().unwrap(), Duration::from_millis(10)).unwrap();

            match *guard
            {
                // Data locked. Requesting data
                State::RENDERING => 
                {
                    *guard = State::REQUEST; 
                    conv.notify_one();
                },
                // Data is read and operation could be performed
                State::READING => 
                {
                    let locked = data.lock().unwrap();

                    f(&locked.0, &locked.1);

                    *guard = State::RENDERING; 
                    conv.notify_one();
                },
                // Data is still locked
                State::REQUEST => {},
            };
        }
    }
    pub fn change_env(&self, new_env: RenderEnv)
    {
        self.render_settings.send(new_env).unwrap();
    }
    pub fn get_render_time(&self) -> Duration
    {
        let guard = self.telemetry.lock().unwrap();

        guard.1
    }
    
    pub fn get_time_per_sample(&self) -> Duration
    {
        let guard = self.telemetry.lock().unwrap();

        guard.1.div_f32(guard.0 as f32)
    }
    pub fn is_done(&self) -> bool
    {
        let guard = self.telemetry.lock().unwrap();
        return guard.0 >= self.max_samples;
    }
}

pub fn trace(surface: &mut Vec<glm::Vec3>, camera: &Camera, world: &Primitive, img_size: (usize, usize), samples: i32, bouces: usize)
{
    surface
    .par_iter_mut()
    .enumerate()
    .for_each(
        |(i, pixel)|
            {
                let mut rng = rand::thread_rng();
                
                let xy: glm::Vec2 = get_index(i, &img_size).into();   

                let mut color = glm::Vec3::default();
                
                for _ in 0..samples
                {
                    let uv = gen_uv(&(xy + rnd_random_vec(&mut rng)), &img_size);
                    let ray = camera.gen_ray(&uv);
                    
                    color += shade(&ray, world, bouces, &mut rng);
                }

                *pixel += color; 
            }
    )
}



