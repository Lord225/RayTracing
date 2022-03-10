#pragma once
#include <glm.hpp>

class ray
{
public:
	glm::vec3 origin;
	glm::vec3 dir;

	ray(glm::vec3 orig, glm::vec3 dir) : origin(orig), dir(dir) {}
	ray(ray&&) = default;
	ray& operator=(ray&&) = default;

	glm::vec3 at(float t) const
	{
		return origin + dir * t;
	}
};

