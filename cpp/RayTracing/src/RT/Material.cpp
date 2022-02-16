#include "Material.h"

bool Diffuse::scatter(const ray& in_ray, const Record& surface, glm::vec3& attenuation, ray& out_ray, std::mt19937& random) const
{
	glm::vec3 dir = surface.norm + rnd_unit_sphere(random);
	out_ray = ray(surface.pos, dir);
	attenuation = albedo;
	return true;
}

bool Metalic::scatter(const ray& in_ray, const Record& surface, glm::vec3& attenuation, ray& out_ray, std::mt19937& random) const
{
    out_ray = ray(surface.pos, glm::reflect(glm::normalize(in_ray.dir), surface.norm) + fuzz * rnd_unit_sphere(random));
    attenuation = albedo;
    return glm::dot(out_ray.dir, surface.norm) > 0;
}

bool Refract::scatter(const ray& in_ray, const Record& surface, glm::vec3& attenuation, ray& out_ray, std::mt19937& random) const
{
	float refraction_ratio = surface.front_face ? (1.0f / ior) : ior;
	out_ray = ray(surface.pos, glm::refract(glm::normalize(in_ray.dir), surface.norm, refraction_ratio));
	attenuation = { 1.0, 1.0, 1.0 };
	return true;
}