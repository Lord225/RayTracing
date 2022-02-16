use nalgebra_glm as glm;

#[derive(Clone, Copy, Default, Debug)]
pub struct Ray
{
    pub orig: glm::Vec3,
    pub dir: glm::Vec3
}

impl Ray
{
    pub fn get_at(&self, t: f32) -> glm::Vec3
    {
        return self.orig + self.dir*t;
    }
}