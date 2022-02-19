use nalgebra_glm as glm;
use rand::prelude::ThreadRng;

use crate::ray_tracer::ray::Ray;
use crate::utils::rnd_on_unit_sphere;

use super::HitRecord;

#[derive(Clone, Debug)]
pub enum Material
{
    Diffuse
    {
        albedo: glm::Vec3,
    },
    Metalic
    {
        albedo: glm::Vec3,
        fuzz: f32,
    },
    Refract
    {
        ior: f32,
    }
}


impl Material
{
    pub fn new_diffuse(albedo: &glm::Vec3) -> Self
    {
        Material::Diffuse{albedo: *albedo}
    }
    pub fn new_metalic(albedo: &glm::Vec3, fuzz: f32) -> Self
    {
        Material::Metalic{albedo: *albedo, fuzz: fuzz}
    }
    pub fn new_refract(ior: f32) -> Self
    {
        Material::Refract{ior: ior}
    }

    pub fn scatter(&self, ray: &Ray, hit: &HitRecord, rng: &mut ThreadRng) -> (Option<Ray>, glm::Vec3)
    {
        match self
        {
            Material::Diffuse { albedo } => scatter_diffuse(ray, hit, albedo, rng),
            Material::Metalic { albedo, fuzz } => scatter_metalic(ray, hit, albedo, fuzz, rng),
            Material::Refract { ior } => scatter_refract(ray, hit, ior, rng),
        }
    }
}

fn scatter_diffuse(_: &Ray, hit: &HitRecord, albedo: &glm::Vec3, rng: &mut ThreadRng) -> (Option<Ray>, glm::Vec3)
{
    let out = Ray{orig: hit.pos, dir: hit.norm + rnd_on_unit_sphere(rng)};

    (Some(out), *albedo)
}

fn scatter_metalic(ray: &Ray, hit: &HitRecord, albedo: &glm::Vec3, fuzz: &f32, rng: &mut ThreadRng) -> (Option<Ray>, glm::Vec3)
{
    let out = Ray
    {
        orig: hit.pos, 
        dir: glm::reflect_vec
        (
            &ray.dir.normalize(), 
            &hit.norm) + rnd_on_unit_sphere(rng).component_mul(&glm::vec3(*fuzz, *fuzz, *fuzz)
        )
    };

    (Some(out), *albedo)
}
fn scatter_refract(ray: &Ray, hit: &HitRecord, ior: &f32, _: &mut ThreadRng) -> (Option<Ray>, glm::Vec3)
{
    let refraction = if hit.face { 1.0f32/ior} else { *ior }; 

    (Some(Ray{orig: hit.pos, dir: glm::refract_vec(&ray.dir, &hit.norm, refraction)}), [1.0, 1.0, 1.0].into())
}