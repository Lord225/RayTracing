use nalgebra_glm as glm;
use rand::{prelude::ThreadRng, Rng};

#[repr(packed(4))]
#[allow(dead_code)]
struct Color
{
    b: u8,
    g: u8,
    r: u8,
    a: u8,
}

pub fn vec3_to_color(v: &glm::Vec3, scale: f32) -> u32
{
    const MAX: f32 = 255.999f32;
    
    let color = Color
    {
        r: (v.x*MAX/scale) as u8,
        g: (v.y*MAX/scale) as u8, 
        b: (v.z*MAX/scale) as u8, 
        a: 0
    };

    // Safty size of `Color` is 32 bit
    unsafe { std::mem::transmute(color) } 
}

pub fn rnd_in_unit_sphere(rng: &mut ThreadRng) -> glm::Vec3
{
    loop 
    {
        let vec = glm::vec3(rng.gen_range(-1.0f32..1.0), rng.gen_range(-1.0f32..1.0), rng.gen_range(-1.0f32..1.0));
        if vec.magnitude_squared() <= 1.0
        {
            return vec;
        }
    }
}

pub fn rnd_on_unit_sphere(rng: &mut ThreadRng) -> glm::Vec3
{
    rnd_in_unit_sphere(rng).normalize()
}

pub fn rnd_random_vec(rng: &mut ThreadRng) -> glm::Vec2
{
    [rng.gen_range(0.0f32..1.0), rng.gen_range(0.0f32..1.0)].into()
}