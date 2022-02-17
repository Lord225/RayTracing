import numpy as np


class RayWrapper:
    def __init__(self, arr: np.ndarray):
        self.dir = arr[0]
        self.pos = arr[1]