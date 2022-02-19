use std::default;
use nalgebra_glm as glm;
use crate::ray_tracer::hittable::{Hittable, HitRecord, AABB};
use super::{primitive::Primitive, vector::Vector};

#[derive(Clone, Debug)]
struct BvhTree
{
    left: Option<Box<BvhTree>>,
    right: Option<Box<BvhTree>>,
    aabb: AABB,
    data: Option<Primitive>,
}

#[derive(Clone, Debug)]
pub struct Bvh
{
    tree: Box<BvhTree>,
}


impl Bvh
{
    pub fn get_aabb(objects: &[Primitive]) -> AABB
    {
        let mut aabb = crate::ray_tracer::hittable::AABB{min: default::default(), max: default::default()};

        for el in objects.iter()
        {
            let el_aabb = el.get_aabb();
            
            aabb.min = glm::min2(&aabb.min, &el_aabb.min);
            aabb.max = glm::max2(&aabb.max, &el_aabb.max);
        }
        return aabb;
    }
    fn recursive(objects: &mut [Primitive]) -> BvhTree
    {
        if objects.len() == 1
        {
            let x = (*objects.first().unwrap()).clone();
            return BvhTree {left: None, right: None, aabb: x.get_aabb(), data: Some(x)};
        }
        
        let list_aabb = Bvh::get_aabb(objects);

        let size = list_aabb.size();
        let highest = glm::comp_max(&size);
        
        use core::cmp::Ordering::Equal;

        if highest == size.x
        {
            objects.sort_unstable_by(|a, b| a.get_aabb().mid().x.partial_cmp(&b.get_aabb().mid().x).unwrap_or(Equal))
        }
        else if highest == size.y 
        {
            objects.sort_unstable_by(|a, b| a.get_aabb().mid().y.partial_cmp(&b.get_aabb().mid().y).unwrap_or(Equal))
        }
        else 
        {
            objects.sort_unstable_by(|a, b| a.get_aabb().mid().z.partial_cmp(&b.get_aabb().mid().z).unwrap_or(Equal))
        }

        let (left, right) =  objects.split_at_mut(objects.len()/2);

        let left = if left.len() != 0 { 
            Some(
                Box::new(
                    Bvh::recursive(
                        left
                    )
                )
            ) 
        } 
        else { 
            None 
        };
        let right = if right.len() != 0 { 
            Some(
                Box::new(
                    Bvh::recursive(
                            right
                    )
                )
            ) 
        } 
        else 
        { 
            None 
        };


        BvhTree{ left: left, right: right, aabb: list_aabb, data: None}
    }

    pub fn build_bvh(mut objects: Vector) -> Self
    {
        Self{tree: Box::new(Self::recursive(&mut objects.list))}
    }
}

impl Hittable for Bvh
{
    fn intersect<'a>(&'a self, ray: &crate::ray_tracer::ray::Ray, min: f32, max: f32) -> Option<HitRecord<'a>> 
    {
        let mut bvh = Vec::<&Box<BvhTree>>::with_capacity(16);
        bvh.push(&self.tree);
        let mut closest = max;
        let mut closest_hit: Option<HitRecord<'a>> = None;

        while !bvh.is_empty()
        {
            let el = bvh.pop().unwrap();

            if el.aabb.intersect(&ray)
            {
                if let Some(obj) = &el.data
                {
                    if let Some(hit) = obj.intersect(ray, min, closest)
                    {
                        if hit.dis < closest
                        {
                            closest = hit.dis;
                            closest_hit = Some(hit);
                        }
                    }
                }
                if let Some(l) = &el.left
                {
                    bvh.push(l);
                }
                if let Some(r) = &el.right
                {
                    bvh.push(r);
                }
            }
        }
        return closest_hit;
    }

    fn get_aabb(& self) -> AABB 
    {
        return self.tree.aabb;
    }
}



impl Into<Primitive> for Bvh
{
    fn into(self) -> Primitive {
        Primitive::Bvh(self)
    }
}