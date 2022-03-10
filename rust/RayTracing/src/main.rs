#![feature(default_free_fn)]

use std::default::default;
use std::mem::size_of;
use minifb::*;
use nalgebra_glm as glm;
use rand::Rng;
use ray_tracer::hittable::material::Material;
use ray_tracer::hittable::primitives::{sphere::Sphere, vector::Vector, primitive::Primitive, bvh::Bvh};
use rayon::prelude::*;

mod ray_tracer;
mod utils;

const WIDTH: usize = 1280;
const HEIGHT: usize = 720;
const SAMPLES: usize = 32;
const BOUCES: usize = 32;

fn update_buffer(buffer: &mut Vec<u32>, data: &Vec<glm::Vec3>, samples: f32)
{
    buffer
    .par_iter_mut()
    .zip(data)
    .for_each(
        |(out, input)|
        {
            *out = utils::vec3_to_color(input, samples)
        }
    );
}

#[allow(dead_code)]
fn get_random_material() -> Material
{
    let mut rng = rand::thread_rng();
    let mat_type: i32 = rng.gen_range(0..3);
    match mat_type
    { 
        0 => Material::new_diffuse(&rng.gen::<[f32; 3]>().into()),
        1 => Material::new_metalic(&rng.gen::<[f32; 3]>().into(), rng.gen::<f32>().into()),
        _ => Material::new_refract(1.0 + rng.gen::<f32>()),
    }
}

fn generate_scene<const USE_BVH: bool, const SCENE: usize> () -> Primitive
{
    if SCENE == 0
    {
        let data: Vector = Vector::from(
            vec!
            [
                Sphere{origin: [0.0f32, 0.0,   0.0].into(), radius: 0.5f32,   mat: Box::new(Material::new_diffuse(&[0.7f32, 0.3f32, 0.3f32].into()))}.into(),
                Sphere{origin: [0.0f32, 100.5, 0.0].into(), radius: 100f32,   mat: Box::new(Material::new_diffuse(&[0.21f32, 0.37f32, 0.69f32].into()))}.into(), 
                Sphere{origin: [0.0f32, 0.2,  -1.0].into(), radius: 0.3,      mat: Box::new(Material::new_metalic(&[0.8f32, 0.8, 0.8].into(), 0.0f32))}.into(),
                Sphere{origin: [0.0f32, 0.0,   1.0].into(), radius: 0.4,      mat: Box::new(Material::new_refract(10.0f32))}.into() 
            ]
            )
            .into();

            if USE_BVH
            {
                Bvh::build_bvh(data.into()).into()
            }
            else
            {
                data.into()
            }
    }
    else
    {
        const SPHERES: usize = 16;

        let mut spheres = Vec::with_capacity(SPHERES*SPHERES*SPHERES);
        let mut rng = rand::thread_rng();
    
        for x in 0..SPHERES {
            for y in 0..SPHERES {
                for z in 0..SPHERES {
                    spheres.push(
                        Sphere
                        {
                            origin: [2.0*x as f32 +rng.gen::<f32>()-0.5 , 2.0*y as f32 - (SPHERES/2) as f32 + rng.gen::<f32>() -0.5, 2.0*z as f32 - (SPHERES/2) as f32 + rng.gen::<f32>() -0.5].into(),
                            radius: 0.3f32,   
                            mat:  Box::new(get_random_material())
    
                        }.into()
                    );
                }
            }
        }
        let data: Vector = spheres.into();

        if USE_BVH
        {
            Bvh::build_bvh(data.into()).into()
        }
        else
        {
            data.into()
        }
    }
}

fn main() 
{
    
    let mut window = Window::new(
        "RT Demo (Rust)",
        WIDTH,
        HEIGHT,
   WindowOptions::default()
    ).unwrap_or_else(|e| {
        panic!("{}", e);
    });

    window.limit_update_rate(Some(std::time::Duration::from_micros(16600)));
    
    let mut buffer: Vec<u32> = vec![0; WIDTH * HEIGHT];
    let data: Vec<glm::Vec3> = vec![glm::Vec3::default(); WIDTH * HEIGHT];
    
    let mut camera_pos = [-2.0f32, 0.0, 0.0].into();
    let camera = ray_tracer::camera::Camera::new(&camera_pos, &[1.0f32, 0.0, 0.0].into(), WIDTH as f32 / HEIGHT as f32, 1f32);

    
    let world: Primitive = generate_scene::<false, 0>().into();

    let size = size_of::<Primitive>();
    println!("{size}");

    let mut engine = ray_tracer::engine::RayTracer::new(data, camera, world, (WIDTH, HEIGHT), SAMPLES, BOUCES);

    let mut update = false;
    let mut is_done_unlocked = true;
    let mut frame_count = 0;

    while window.is_open() && !window.is_key_down(Key::Escape) 
    {
        frame_count += 1;
        
        window.update_with_buffer(&buffer, WIDTH, HEIGHT).unwrap();

        engine.request_data(|data, iters| update_buffer(&mut buffer, data, *iters as f32));
        
        if window.is_key_down(Key::D)
        {
            camera_pos.z += 0.1;
            update = true;
        }
        if window.is_key_down(Key::A)
        {
            camera_pos.z -= 0.1;
            update = true;
        }
        if window.is_key_down(Key::W)
        {
            camera_pos.x += 0.1;
            update = true;
        }
        if window.is_key_down(Key::S)
        {
            camera_pos.x -= 0.1;
            update = true;
        }

        if update
        {
            let camera = ray_tracer::camera::Camera::new(&camera_pos, &[1.0f32, 0.0, 0.0].into(), WIDTH as f32 / HEIGHT as f32, 1.5f32);

            engine.change_env(ray_tracer::engine::RenderEnv { camera: Some(camera), ..default()  });
            update = false;
        }
        
        if frame_count % 10 == 0 && engine.is_done() == false
        {
            let sample_time = engine.get_time_per_sample();
            println!("Time per sample: {sample_time:?}");
        }
        
        if engine.is_done() == true
        {
            if is_done_unlocked
            {
                is_done_unlocked = false;

                let total_time = engine.get_render_time();
                println!("Done. Time: {total_time:?}");
            }
        }
        else 
        {
            is_done_unlocked = true;
        }
    }

}
