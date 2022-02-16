#pragma once
#include <optional>
#include <glm.hpp>
#include <memory>
#include "../Ray.h"
#include "../Material.h"

class IMaterial;

struct Record
{
	glm::vec3 pos;
	glm::vec3 norm;
	float dis;
	bool front_face;
	std::shared_ptr<IMaterial> mat;

	static Record from(const glm::vec3& pos, const glm::vec3& norm, float dis, const ray& r, std::shared_ptr<IMaterial> mat)
	{
		auto rec = Record{ pos, norm, dis, true, mat };
		if (dot(r.dir, norm) > 0)
		{
			rec.front_face = false;
			rec.norm = -norm;
		}
		return rec;
	}
};

class IHittable
{
public:
	virtual std::optional<Record> intersect(const ray& ray, float min, float max) const = 0;
};

