use crate::ray_tracer::hittable::HitRecord;
use crate::ray_tracer::hittable::Hittable;
use crate::ray_tracer::ray::Ray;
use super::sphere;
use super::vector;
use super::bvh;

#[derive(Clone, Debug)]
pub enum Primitive
{
    Sphere(sphere::Sphere),
    Vector(vector::Vector),
    Bvh(bvh::Bvh),
}

impl Hittable for Primitive
{
    fn intersect<'a>(&'a self, ray: &Ray, min: f32, max: f32) -> Option<HitRecord<'a>> 
    {
        match self 
        {
            Primitive::Sphere(sphere) => sphere.intersect(&ray, min, max),
            Primitive::Vector(list ) => list.intersect(&ray, min, max),
            Primitive::Bvh(data) => data.intersect(&ray, min, max),
        }
    }

    fn get_aabb(& self) -> crate::ray_tracer::hittable::AABB {
        match self 
        {
            Primitive::Sphere(sphere) => sphere.get_aabb(),
            Primitive::Vector(list ) => list.get_aabb(),
            Primitive::Bvh(data) => data.get_aabb(),
        }
    }
}
