from ray_tracer.ray import RayWrapper
from ray_tracer.primitive.base import Hittable
from ray_tracer.camera import Camera
import multiprocessing as mp
import numpy as np
from progress.bar import Bar
import cv2 as cv

def sky(ray):
    t = (ray[0][1]+1)/2
    a = np.array([1.0, 1.0, 1.0])
    b = np.array([0.5, 0.7, 1.0])
    return t*a + (1-t)*b

    
def shade(ray: np.ndarray, world: Hittable, bounces):
    if bounces == 0:
        return np.zeros((3))

    hit = world.intersect(ray, 0.001, np.inf)
    
    if hit is not None:
        scattered, col = hit.mat.scatter(RayWrapper(ray), hit)
        if scattered is None:
            return np.zeros((3))
        return col*shade(scattered, world, bounces-1)
    else:
        return sky(ray)

class Shade:
    def __init__(self, world, bounces):
        self.world = world
        self.bounces = bounces

    def __call__(self, _, ray):
        return shade(ray, self.world, self.bounces)
    
def trace(surf: np.ndarray, camera: Camera, world, samples, bounces):
    pool = mp.Pool(processes=1)
    h, w, _ = surf.shape
    
    xx = np.arange(0.0, float(w))
    yy = np.arange(0.0, float(h))
    xv, yv = np.meshgrid(xx, yy)
    xy = np.stack((xv, yv), axis=2)
    

    bar = Bar('Sampling...', max=samples, suffix='%(index)d/%(max)d eta: %(eta)ds')
    print("Processing..")
    for samples in range(samples):
        seed = np.random.random((h, w, 2))
        uvs = (xy+seed)/np.array([w, h])
        uvs = (uvs-0.5)*2
        uvs = np.reshape(uvs, (-1, 2))
        rays = camera.gen_rays(uvs)

        colors = pool.starmap_async(Shade(world, bounces), enumerate(rays), callback=lambda _: bar.next())

        colors = np.reshape(np.array(colors.get()), (h, w, 3))
        surf += colors

        cv.imshow("RT Demo (Python)", surf[:, :, [2, 1, 0]]/(samples+1))
        cv.waitKey(1)
    return surf













