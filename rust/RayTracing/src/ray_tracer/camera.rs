use nalgebra_glm as glm;
use super::ray;

#[derive(Default)]
pub struct Camera
{
    inv: glm::Mat4,
    origin: glm::Vec3,
}

impl Camera
{
    pub fn gen_ray(&self, uv: &glm::Vec2) -> ray::Ray
    {
        let screen_pos = glm::vec4(uv.x, uv.y, 1.0, 1.0);
		let world_pos = self.inv * screen_pos;

		let dir = glm::normalize(&world_pos.xyz());
		let orig = self.origin; 

        ray::Ray { orig: orig, dir: dir }
    }
    
    pub fn new(pos: &glm::Vec3, dir: &glm::Vec3, aspectratio: f32, focal: f32) -> Self
    {
        let proj = glm::perspective_fov(focal, aspectratio, 1.0, 0.2, 1.0);
        let view = glm::look_at(&[0.0f32, 0.0f32, 0.0f32].into(), dir, &[0.0f32, 1.0, 0.0].into());
        
        let inv = glm::inverse(&(proj*view));

        Self{inv: inv, origin: *pos}
    }
}



