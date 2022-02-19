from typing import Optional, Tuple
from ray_tracer.ray import RayWrapper 
from ray_tracer.primitive.base import Material, Hit 
import numpy as np
import numba as nb

@nb.njit
def rnd_on_unit_sphere() -> np.ndarray:
    while True:
        v = np.random.uniform(0, 1, (3))
        if np.sum(v*v) <= 1:
            return v

@nb.njit
def reflect(v: np.ndarray, n: np.ndarray):
    return v - 2*np.sum(v*n)*n

@nb.njit
def refract(uv: np.ndarray, n: np.ndarray, etai_over_etat: float):
    cos_theta = min(float(np.sum((-uv)*n)), 1.0)
    r_out_perp = etai_over_etat * (uv + cos_theta*n)
    r_out_parallel = -np.math.sqrt(np.abs(1.0 - np.sum(r_out_perp*r_out_perp))) * n
    return r_out_perp + r_out_parallel


class Diffuse(Material):
    def __init__(self, albedo):
        self.albedo = np.array(albedo)
    def scatter(self, _: RayWrapper, hit: Hit) -> Tuple[Optional[np.ndarray], np.ndarray]:
        ray_out = np.array([rnd_on_unit_sphere()+hit.norm, hit.pos])
        return (ray_out, self.albedo)

class Metalic(Material):
    def __init__(self, albedo):
        self.albedo = np.array(albedo)
    def scatter(self, ray: RayWrapper, hit: Hit) -> Tuple[Optional[np.ndarray], np.ndarray]:
        reflected = reflect(ray.dir, hit.norm)
        ray_out = np.array([reflected, hit.pos])
        return (ray_out, self.albedo)

class Refract(Material):
    def __init__(self, ior):
        self.ior = ior
    def scatter(self, ray: RayWrapper, hit: Hit) -> Tuple[Optional[np.ndarray], np.ndarray]:
        refraction_ratio = 1/self.ior if hit.face else self.ior
        refracted = np.array([refract(ray.dir, hit.norm, refraction_ratio), hit.pos])
        return (refracted, np.array([1,1,1]))