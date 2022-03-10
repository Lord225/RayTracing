#pragma once
#include "../../Utils/VecStuff.h"
#include "../Camera/Ray.h"
#include <glm.hpp>
#include <random>

#include "../Primitives/Hittable.h"

namespace Primitives
{
    struct Record;
}

namespace Mat
{
    class IMaterial
    {
    public:
        virtual bool scatter(const ray& in_ray, const Primitives::Record& surface, glm::vec3& attenuation, ray& out_ray, std::mt19937& random) const = 0;
    };


    class Diffuse : public IMaterial
    {
    public:
        Diffuse(const glm::vec3& albedo) : albedo(albedo) {}
        virtual bool scatter(const ray& in_ray, const Primitives::Record& surface, glm::vec3& attenuation, ray& out_ray, std::mt19937& random) const override;
        glm::vec3 albedo;
    };


    class Metalic : public IMaterial
    {
    public:
        Metalic(glm::vec3 albedo, float fuzz) : albedo(albedo), fuzz(fuzz) {}
        virtual bool scatter(const ray& in_ray, const Primitives::Record& surface, glm::vec3& attenuation, ray& out_ray, std::mt19937& random) const override;
        glm::vec3 albedo;
        float fuzz;
    };

    class Refract : public IMaterial
    {
    public:
        Refract(float ior) : ior(ior) {}
        virtual bool scatter(const ray& in_ray, const Primitives::Record& surface, glm::vec3& attenuation, ray& out_ray, std::mt19937& random) const override;
        float ior;
    };
}