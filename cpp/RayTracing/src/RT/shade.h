#pragma once
#include "Ray.h"
#include "Primitives/Hittable.h"
#include <glm.hpp>
#include <random>
#include "../Utils/VecStuff.h"

glm::vec3 sky(const ray& r);

glm::vec3 gen_color(const ray& r, const IHittable& world, std::mt19937& random, int depth);