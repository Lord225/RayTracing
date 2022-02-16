use crate::ray_tracer::hittable::HitRecord;
use crate::ray_tracer::hittable::Hittable;
use crate::ray_tracer::ray::Ray;
use super::sphere;
use super::vector;

pub enum Primitive
{
    Sphere(sphere::Sphere),
    Vector(vector::Vector)
}

impl Hittable for Primitive
{
    fn intersect<'a>(&'a self, ray: &Ray, min: f32, max: f32) -> Option<HitRecord<'a>> 
    {
        match self 
        {
            Primitive::Sphere(sphere) => sphere.intersect(&ray, min, max),
            Primitive::Vector(list ) => list.intersect(&ray, min, max),
        }
    }
}
