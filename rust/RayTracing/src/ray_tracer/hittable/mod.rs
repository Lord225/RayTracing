pub mod material;
pub mod primitives;

use nalgebra_glm as glm;
use super::ray::Ray;

pub trait Hittable
{
    fn intersect<'a>(&'a self, ray: &Ray, min: f32, max: f32) -> Option<HitRecord<'a>>;
}

pub struct HitRecord<'a>
{
    pub pos: glm::Vec3,
    pub norm: glm::Vec3,
    pub dis: f32,
    pub face: bool,
    pub mat: &'a material::Material
}

impl<'a> HitRecord<'a>
{
    pub fn new(pos: &glm::Vec3, norm: &glm::Vec3, dis: f32, ray: &Ray, mat: &'a material::Material) -> Self
    {
        if glm::dot(&ray.dir, norm) > 0.0
        {
            Self {pos: *pos, norm: -norm, dis: dis, mat: mat, face: false}
        }
        else 
        {
            Self {pos: *pos, norm: *norm, dis: dis, mat: mat, face: true}
        }
    }
}
