#include "Material.h"

bool Mat::Diffuse::scatter(const ray& in_ray, const Primitives::Record& surface, glm::vec3& attenuation, ray& out_ray, std::mt19937& random) const
{
	glm::vec3 dir = surface.norm + Utils::Vec3::rnd_unit_sphere(random);
	out_ray = ray(surface.pos, dir);
	attenuation = albedo;
	return true;
}

bool Mat::Metalic::scatter(const ray& in_ray, const Primitives::Record& surface, glm::vec3& attenuation, ray& out_ray, std::mt19937& random) const
{
    out_ray = ray(surface.pos, glm::reflect(glm::normalize(in_ray.dir), surface.norm) + fuzz * Utils::Vec3::rnd_unit_sphere(random));
    attenuation = albedo;
    return glm::dot(out_ray.dir, surface.norm) > 0;
}

bool Mat::Refract::scatter(const ray& in_ray, const Primitives::Record& surface, glm::vec3& attenuation, ray& out_ray, std::mt19937& random) const
{
	float refraction_ratio = surface.front_face ? (1.0f / ior) : ior;
	out_ray = ray(surface.pos, glm::refract(glm::normalize(in_ray.dir), surface.norm, refraction_ratio));
	attenuation = { 1.0, 1.0, 1.0 };
	return true;
}