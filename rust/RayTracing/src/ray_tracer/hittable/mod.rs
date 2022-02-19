pub mod material;
pub mod primitives;

use nalgebra_glm as glm;
use super::ray::Ray;

#[derive(Clone, Copy, Default, Debug)]
pub struct AABB
{
    pub min: glm::Vec3,
    pub max: glm::Vec3,
}


pub trait Hittable
{
    fn intersect<'a>(&'a self, ray: &Ray, min: f32, max: f32) -> Option<HitRecord<'a>>;
    
    fn get_aabb(& self) -> AABB;
}


impl AABB 
{
    fn intersect(&self, ray: &Ray) -> bool {
        let dir_frac: glm::Vec3 =  glm::vec3(1.0f32, 1.0, 1.0).component_div(&ray.dir);

        let min_bound = (self.min - ray.orig).component_mul(&dir_frac);
        let max_bound = (self.max - ray.orig).component_mul(&dir_frac);

        let tmin = glm::comp_max(&glm::min2(&min_bound, &max_bound));
        let tmax = glm::comp_min(&glm::max2(&min_bound, &max_bound));
        
        if tmax < 0.0 || tmin > tmax
        {
            return false;
        }
        else
        {
            return true;
        }
    }
    fn mid(&self) -> glm::Vec3
    {
        (self.max + self.min)*0.5f32
    }
    fn size(&self) -> glm::Vec3
    {
        self.max - self.min
    }
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
