# RayTracing

Implemenation of simple RT algorithms in 3 diffrent languages to compare preformance, syntax, toolchains and joy of using writing in it.
**TODO**

## Goal
Implement RT algorithm with Sphere-Ray intersection that works on seperate threads than window, movable camera and 3 diffrent materials (Diffuse, Reflective and Refractive) using most popular tools and techniques. 
**TODO**

## Performance

### Test 1 - 32 Samples, 5 Bounces, 4 Spheres
Summary (`i7 7700k@4.5GHZ`)

|              |  Rust  |  C++  | Python |
|--------------|--------|-------|--------|
| No Threading | 8.28s  | 11.9s |   XD   |
| Threading    | 1.76s  | 5.25s | 1974s  |

### Test 2 - 32 Samples 128 Bounces 32'768 Spheres

|              |  Rust  |  C++  | Python |
|--------------|--------|-------|--------|
| No Threading | x  | x |   x   |
| Threading    | x  | x |   x   |

## How painful it was?

* `C++` - `😡😥😒😐 * * ` - C++ is not fun to use. Small things were iritating, hard to use. Sometimes I had to sacrifice some performence for clarity like using shared_ptr instead of reference. At least I wasn't forced to manage memory manually. On other hand I was able to implement some features with ease because C++ doesnt care about safty of multithreading as much as Rust does.
* `Rust` - `😡😥😒😐🙂` - RT engine was simple to implement and I was able to make everything like I intended to do. But becouse Rust is very overzealous about multithreading - it was pritty hard to get it to work properly, but I do not encounter any bug  along the way unlike with C++ 
* `Python` - `😡 * * * * ` - It would be easy, but becouse I dont want to get old while waiting for python to trace sample scene (1280x720, 32 samples 5 bounces) I had to use numba and numpy - It was fun, but in order to express everything in terms of ndarrays was hard and painfull do debug. If I would not care about performance and make in in pure python - it would be done in matter of second ( rendering still will take ages, It is taking anyway) 

## Whats next

* Implement ray-triangle intersection with AABB optimalization
* Implement CUDA version (C++ supercharged with compute shaders)
* Textures / Other material types

