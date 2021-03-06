#include "VecStuff.h"


glm::vec3 Utils::Vec3::rnd_unit_sphere(std::mt19937& gen)
{
	const std::uniform_real_distribution<float> rand(-1.0f, 1.0f);

	for (size_t i = 0; i < 2; i++)
	{
		glm::vec3 vec(rand(gen), rand(gen), rand(gen));
		if (glm::dot(vec, vec) < 1.0f)
			return vec;
	}
	return glm::vec3(rand(gen) * 0.86, rand(gen) * 0.86, rand(gen) * 0.86);
}

float Utils::Vec3::sqr_lenght(const glm::vec3& val) {
	return glm::dot(val, val);
}
