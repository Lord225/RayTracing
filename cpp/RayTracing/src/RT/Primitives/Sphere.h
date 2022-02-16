#pragma once
#include <glm.hpp>
#include "Hittable.h"
#include "../Ray.h"
#include "../../Utils/VecStuff.h"
#include "../Material.h"

class IMaterial;


class Sphere : public IHittable
{
public:
	glm::vec3 origin;
	float radius;

    std::shared_ptr<IMaterial> mat;

	Sphere(glm::vec3 origin, float radius, std::shared_ptr<IMaterial> mat) : origin(origin), radius(radius), mat(mat) {}

	std::optional<Record> intersect(const ray& ray, float min, float max) const override
	{
        const auto oc = ray.origin - origin;
        const auto a = sqr_lenght(ray.dir);
        const auto half_b = glm::dot(oc, ray.dir);
        const auto c = sqr_lenght(oc) - radius * radius;

        auto discriminant = half_b * half_b - a * c;
        if (discriminant < 0) return {};
        auto sqrtd = sqrt(discriminant);


        auto root = (-half_b - sqrtd) / a;
        if (root < min || max < root) 
        {
            root = (-half_b + sqrtd) / a;
            if (root < min || max < root)
                return {};
        }


        glm::vec3 pos = ray.at(root);
        glm::vec3 norm = (pos - origin) / radius;

        return Record::from(pos, norm, root, ray, mat);
	}
};

