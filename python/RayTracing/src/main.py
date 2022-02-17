import cv2 as cv
import numpy as np
import ray_tracer
import time
from ray_tracer.primitive.sphere import Sphere
from ray_tracer.primitive.array import Array
import ray_tracer.primitive.material as material

REDUCTION = 1
WINDOW_NAME = "RT Demo (Python)"
SIZE = (720//REDUCTION, 1280//REDUCTION, 3)
SAMPLES = 32
BOUNCES = 5

def main():
    
    cv.namedWindow(WINDOW_NAME, cv.WINDOW_KEEPRATIO)

    camera = ray_tracer.camera.Camera(np.array([-1.5,0,0]), np.array([-1, 0.0, 0.0]), SIZE[1]/SIZE[0], 120)
    img = np.zeros(SIZE)
    world = Array(
        [
            Sphere(np.array([0, 0, 0]), 0.5, material.Diffuse([0.7, 0.3, 0.3])),
            Sphere(np.array([0, 100.5, 0]), 100, material.Diffuse([0.21, 0.37, 0.69]))
        ]
    )

    print("Starting")

    start = time.time()
    img = ray_tracer.engine.trace(img, camera, world, SAMPLES, BOUNCES)
    end = time.time()


    print(f"Time: {(end-start)}s")
    img = img[:, :, [2, 1, 0]]/SAMPLES
    
    while True:
        cv.imshow(WINDOW_NAME, img)

        if cv.waitKey(1) == ord('q'):
            break
    
    cv.imwrite("out.png", img)
    cv.destroyAllWindows()


if __name__ == "__main__":
    main()


