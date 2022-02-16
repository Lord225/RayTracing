#pragma once
#include <glm.hpp>
#include <random>

glm::vec3 rnd_unit_sphere(std::mt19937& gen);
float sqr_lenght(const glm::vec3& val);