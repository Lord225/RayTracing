#pragma once
#include <glm.hpp>
#include <random>

#include "../../Utils/VecStuff.h"

#include "../Camera/Ray.h"
#include "../Material/Material.h"
#include "../Primitives/Hittable.h"

glm::vec3 sky(const ray& r);

glm::vec3 gen_color(const ray& r, const Primitives::IHittable& world, std::mt19937& random, int depth);