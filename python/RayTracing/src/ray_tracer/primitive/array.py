from ray_tracer.primitive.base import Hittable, Hit
from typing import Optional, List

class Array(Hittable):
    def __init__(self, list: List[Hittable]):
        self.list = list
    def intersect(self, ray, min, max) -> Optional[Hit]:
        closest = max
        closest_hit = None

        for el in self.list:
            hit = el.intersect(ray, min, closest)
            if hit is not None:
                if hit.dis < closest:
                    closest = hit.dis
                    closest_hit = hit
                    
        return closest_hit


