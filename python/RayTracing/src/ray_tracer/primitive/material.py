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
    

class Diffuse(Material):
    def __init__(self, albedo):
        self.albedo = np.array(albedo)
    def scatter(self, ray: RayWrapper, hit: Hit) -> Tuple[Optional[np.ndarray], np.ndarray]:
        ray_out = np.array([rnd_on_unit_sphere()+hit.norm, hit.pos])
        return (ray_out, self.albedo)