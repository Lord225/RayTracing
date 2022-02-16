#pragma once
#include "Ray.h"
#include "Primitives/Hittable.h"
#include <glm.hpp>
#include <random>
#include "../Utils/VecStuff.h"

struct Record;

class IMaterial
{
public: 
    virtual bool scatter(const ray& in_ray, const Record& surface, glm::vec3& attenuation, ray& out_ray, std::mt19937& random) const = 0;
};


class Diffuse : public IMaterial
{
public:
    Diffuse(const glm::vec3& albedo) : albedo(albedo) {}
    bool scatter(const ray& in_ray, const Record& surface, glm::vec3& attenuation, ray& out_ray, std::mt19937& random) const override;
    glm::vec3 albedo;
};


class Metalic : public IMaterial
{
public:
    Metalic(glm::vec3 albedo, float fuzz) : albedo(albedo), fuzz(fuzz) {}
    bool scatter(const ray& in_ray, const Record& surface, glm::vec3& attenuation, ray& out_ray, std::mt19937& random) const override;
    glm::vec3 albedo;
    float fuzz;
};

class Refract : public IMaterial
{
public:
    Refract(float ior) : ior(ior) {}
    bool scatter(const ray& in_ray, const Record& surface, glm::vec3& attenuation, ray& out_ray, std::mt19937& random) const override;
    float ior;
};