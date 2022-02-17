import numpy as np
import abc
from ray_tracer.ray import RayWrapper
from typing import Optional, Tuple

class Hit:
    def __init__(self, pos: np.ndarray, norm:  np.ndarray, dis: float, ray: RayWrapper, mat: 'Material'):
        self.pos = pos
        self.dis = dis
        self.mat = mat
        
        if np.sum(ray.dir*norm) > 0:
            self.norm = -norm
            self.face = False
        else:
            self.norm = norm
            self.face = True

class Material:
    @abc.abstractmethod
    def scatter(self, ray: RayWrapper, hit: Hit) -> Tuple[Optional[np.ndarray], np.ndarray]:
        pass

class Hittable:
    @abc.abstractmethod
    def intersect(self, ray, min, max) -> Optional[Hit]:
        pass