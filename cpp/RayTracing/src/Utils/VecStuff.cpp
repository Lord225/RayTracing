#include "VecStuff.h"

glm::vec3 rnd_unit_sphere(std::mt19937& gen)
{
	const std::uniform_real_distribution<float> rand(-1.0f, 1.0f);

	while (true)
	{
		glm::vec3 vec(rand(gen), rand(gen), rand(gen));
		if (glm::dot(vec, vec) < 1.0f)
			return vec;
	}
}

float sqr_lenght(const glm::vec3& val) {
	return glm::dot(val, val);
}
