# RayTracing

Implemenation of simple RT algorithms in 3 diffrent languages to compare preformance, syntax, toolchains and joy of using writing in it.

## Goal
Implement RT algorithm with Sphere-Ray intersection that works on seperate threads than window, movable camera and 3 diffrent materials (Diffuse, Reflective and Refractive) using popular tools and techniques:
* `Cpp` - stl, sdl, glm
* `Rust` - Rayon, nalgebra-glm, minifb
* `Python` - numpy, numba, opencv

## Results
### CPP
![cpp](https://user-images.githubusercontent.com/49908210/154814310-3855a67e-09c6-4ff9-a377-056498cbc376.png)
### RUST
![rust](https://user-images.githubusercontent.com/49908210/154814313-7e060d5e-ed6b-4a94-a8fa-cc12c47983ad.png)
### PYTHON
![python](https://user-images.githubusercontent.com/49908210/154814318-3c9203d1-539e-488c-a439-4c64b16162f3.png)

## Performance

### Test 1 - 32 Samples, 5 Bounces, 4 Spheres
Summary (`i7 7700k@4.5GHZ`)

Performance of language may differ bcs diffrent languages have diffrent camera setup. It should not make a big diffrance. 

|              |  Rust  |  C++  | Python |
|--------------|--------|-------|--------|
| No Threading | 8.28s  | 11.9s |   XD   |
| Threading    | 1.76s  | 5.25s | 3156s  |

### Size of files

|              |  Rust  |  C++  | Python |
|--------------|--------|-------|--------|
| Size of workspace | 982M | 359M |   257K   |
| Size of binary    | 400K | 61K + `SDL.dll` | N/A |

## How painful it was?

Every language found other way to give me a grief, but:

* `C++` - `😡😥😒😐🙂 * * ` - C++ is not fun to use. Small things were iritating, hard to use. Sometimes I had to sacrifice some performence for clarity like using shared_ptr instead of reference. At least I wasn't forced to manage memory manually. On other hand I was able to implement some features with ease because C++ doesnt care about safty of multithreading as much as Rust does.
* `Rust` - `😡😥😒😐🙂😀 * ` - RT engine was simple to implement and I was able to make everything like I intended to do. But becouse Rust is very overzealous about multithreading - it was hard to get it to work properly (becouse it has to be strictly correct), but I do not encounter any bug or unexpected (I mean bad) behavior along the way unlike with C++.
* `Python` - `😡 * * * * * ` - It would be easy, but becouse I dont want to get old while waiting for python to trace sample scene (1280x720, 32 samples 5 bounces) I had to use numba and numpy - It was fun, but expressing everything in terms of ndarrays was painful. 

## Whats next

* Implement ray-triangle intersection
* With BVH optimalization
* Implement CUDA version (C++ supercharged with compute shaders)
* Textures / Other material types
* Build C++ version staticly
