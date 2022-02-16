#pragma once
#include "Ray.h"
#include "Primitives/Hittable.h"
#include <glm.hpp>
#include <random>
#include "../Utils/VecStuff.h"

glm::vec3 sky(const ray& r) 
{
    return glm::mix(glm::vec3(1.0f, 1.0f, 1.0f), { 0.5f, 0.7f, 1.0f }, 0.5f * (glm::normalize(r.dir).y + 1.0f));
}

glm::vec3 gen_color(const ray& r, IHittable& world, std::mt19937& random, int depth)
{
    if (depth <= 0)
        return { 0.0f, 0.0f, 0.0f };

    if (auto result = world.intersect(r, 0.001, std::numeric_limits<float>::infinity()))
    {
        glm::vec3 att;
        ray dir({}, {});
        if(result->mat->scatter(r, *result, att, dir, random))
            return att * gen_color(dir, world, random, depth - 1);
        return { 0.0f, 0.0f, 0.0f };
    }

    return sky(r);
}