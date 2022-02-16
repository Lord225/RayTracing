use crate::ray_tracer::hittable::{Hittable, HitRecord, material::Material};
use nalgebra_glm as glm;

use super::primitive::Primitive;

pub struct Sphere
{
    pub origin: glm::Vec3,
    pub radius: f32,
    pub mat: Box<Material>
}

impl Hittable for Sphere
{
    fn intersect<'a>(&'a self, ray: &crate::ray_tracer::ray::Ray, min: f32, max: f32) -> Option<HitRecord<'a>> 
    {
        let oc = ray.orig - self.origin;
        let a = ray.dir.magnitude_squared();
        let half_b = glm::dot(&oc, &ray.dir);
        let c = oc.magnitude_squared() - self.radius*self.radius;

        let discriminant = half_b*half_b - a*c;
        
        if discriminant < 0.0
        {
            return None;
        }
        let sqrtd = discriminant.sqrt();

        let mut root = (-half_b - sqrtd)/a;
        if root < min || max < root
        {
            root = (-half_b + sqrtd) / a;
            if root < min || max < root
            {
                return None;
            }
        }

        let pos = ray.get_at(root);
        let norm = (pos-self.origin) / self.radius;
        
        return Some(HitRecord::new(&pos, &norm, root, ray, self.mat.as_ref()));
    }
}

impl Into<Primitive> for Sphere
{
    fn into(self) -> Primitive {
        Primitive::Sphere(self)
    }
}
