from typing import Optional, Tuple
import numpy as np
import numba as nb
from ray_tracer.primitive.material import Material
from ray_tracer.primitive.base import Hittable, Hit
from ray_tracer.ray import RayWrapper

class Sphere(Hittable):
    @staticmethod
    @nb.njit
    def __sphere_intersection(self_pos: np.ndarray, self_radius: float, ray_dir:np.ndarray, ray_pos:np.ndarray, min: float, max: float) -> Optional[Tuple[np.ndarray, np.ndarray, float]]:
        oc = ray_pos - self_pos
        a = np.sum(ray_dir*ray_dir)
        half_b = np.sum(oc*ray_dir)
        c = np.sum(oc*oc) - self_radius**2
        
        discriminant = half_b*half_b - a*c

        if discriminant < 0.0:
            return None
        sqrtd = discriminant**0.5

        root = (-half_b - sqrtd) / a
        if root < min or max < root:
            root = (-half_b + sqrtd) / a
            if root < min or max < root:
                return None
        
        pos = ray_pos + root * ray_dir
        norm = (pos-self_pos) / self_radius

        return (pos, norm, root)
    def __init__(self, pos: np.ndarray, radius: float, mat: Material):
        self.pos = pos
        self.radius = radius
        self.mat = mat
    def intersect(self, ray, min, max) -> Optional[Hit]:
        ray = RayWrapper(ray)

        data = Sphere.__sphere_intersection(self.pos, self.radius, ray.dir, ray.pos, min, max)

        if data is None:
            return None
        else:
            pos, norm, root = data
            return Hit(pos, norm, root, ray, self.mat)
