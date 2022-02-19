use crate::ray_tracer::hittable::{Hittable, HitRecord};
use super::primitive::{Primitive};
use super::bvh::Bvh;


#[derive(Clone, Debug)]
pub struct Vector
{
    pub list: Vec<Primitive>
}

impl Hittable for Vector
{
    fn intersect<'a>(&'a self, ray: &crate::ray_tracer::ray::Ray, min: f32, max: f32) -> Option<HitRecord<'a>> 
    {
        let mut closest = max;
        let mut closest_hit: Option<HitRecord<'a>> = None;

        for el in self.list.iter()
        {
            if let Some(hit) = el.intersect(ray, min, closest)
            {
                if hit.dis < closest
                {
                    closest = hit.dis;
                    closest_hit = Some(hit);
                }
            }
        }
        
        closest_hit
    }

    fn get_aabb(& self) -> crate::ray_tracer::hittable::AABB 
    {

        return Bvh::get_aabb(&self.list);
    }
}

impl From<Vec<Primitive>> for Vector
{
    fn from(vec: Vec<Primitive>) -> Self {
        Self{list: vec}
    }
}
impl Into<Primitive> for Vector
{
    fn into(self) -> Primitive {
        Primitive::Vector(self)
    }
}