#pragma once
#include "../Utils/SurfaceWrapper.h"
#include "Camera.h"
#include "Ray.h"
#include "Primitives/Hittable.h"
#include <glm.hpp>
#include <optional>
#include <tuple>
#include <iostream>
#include <vector>
#include <random>
#include "shade.h"
#include "../Utils/VecStuff.h"
#include "../Utils/thread_pool.hpp"



class RayTracer {
    int _iterations = 0;
    int _max_iterations = 0;
    int max_bounces = 3;
    int max_workers = 8;

    using real_milliseconds = std::chrono::duration<double, std::ratio<1, 1000>>;
    real_milliseconds total_render_time = real_milliseconds(0.0);

    Camera camera;
    IHittable& world;
    SurfaceWrapper& pixels;

    std::uniform_real_distribution<float> uniform;
    std::mt19937 random;
    
    std::vector<glm::vec3> image;
    glm::vec3& getPixel(size_t x, size_t y)
    {
        return image[x + pixels.w() * y];
    }

    std::thread render_thread;
    void render_loop()
    {
        using namespace std::chrono;
        thread_pool pool(max_workers);
        while (should_run)
        {
            if (_iterations <= _max_iterations)
            {
                auto start = high_resolution_clock::now();

                pool.parallelize_loop(0, image.size(), [&](const int& a, const int& b) { trace_indexes(1, a, b); });
                pool.wait_for_tasks();

                auto end = high_resolution_clock::now();

                total_render_time += duration_cast<real_milliseconds>(end - start);
                _iterations += 1;
            }
            else
            {
                std::this_thread::yield();
            }

            if (_update_surface)
            {
                std::unique_lock<std::mutex> lk(m);
                _update_surface = false;
                _surface_lock.notify_one();
                _working_thread_ready = true;
                _surface_lock.wait(lk);
            }

            if (_update_camera)
            {
                reset();
                camera = new_camera;
                _update_camera = false;
            }
        }
    }

    void update_surface()
    {
        float ratio = _iterations;
        for (int j = 0; j < pixels.h(); ++j)
        {
            for (int i = 0; i < pixels.w(); ++i)
            {
                pixels.setPixel(i, j, getPixel(i, j) / ratio);
            }
        }
    }
    void reset()
    {
        for (auto& px : image)
        {
            px = { 0.0f, 0.0f, 0.0f };
        }
        _iterations = 0;
        total_render_time = real_milliseconds(0);
    }

    void trace_indexes(int samples, int from, int to)
    {
        for (size_t i = from; i < to; i++)
        {
            int x = i % pixels.w();
            int y = i / pixels.w();

            glm::vec3 color = { 0.0f, 0.0f, 0.0f };

            for (size_t sample = 0; sample < samples; sample++)
            {
                const float u = (((float)y + uniform(random)) / (pixels.h() - 1) - 0.5f) * 2.0f;
                const float v = (((float)x + uniform(random)) / (pixels.w() - 1) - 0.5f) * 2.0f;

                auto r = camera.genray({ u, v });

                color += gen_color(r, world, random, max_bounces);
            }
            getPixel(x, y) += color;
        }
    }

    std::mutex m;
    bool _working_thread_ready = false;
    std::atomic_bool _update_surface;
    std::condition_variable _surface_lock;
    std::atomic_bool _update_camera;
    Camera new_camera;

    bool should_run = true;
public:

    int iterations()
    {
        return _iterations;
    }

    real_milliseconds get_total_render_time()
    {
        return total_render_time;
    }
    real_milliseconds get_time_per_sample()
    {
        return get_total_render_time() / (iterations()+1);
    }
    bool is_done()
    {
        return _iterations >= _max_iterations;
    }

    RayTracer(SurfaceWrapper& pixels, IHittable& world, Camera camera, int max_iterations, int max_bounces, int max_workers = 8) : 
        camera(camera), 
        world(world),
        _max_iterations(max_iterations),
        pixels(pixels),
        uniform(0.0f, 1.0f),
        max_bounces(max_bounces),
        max_workers(max_workers),
        new_camera(camera)
    {
        image.resize(pixels.w() * pixels.h());
        render_thread = std::thread([&] () { render_loop(); });
    }

    void request_surface_update()
    {
        std::unique_lock<std::mutex> lk(m);
        if (_surface_lock.wait_for(lk, std::chrono::milliseconds(10), [&]() { return _working_thread_ready; }))
        {
            update_surface();
            _surface_lock.notify_all();
            _working_thread_ready = false;
        }
        else
        {
            _update_surface = true;
        }
    }

    void request_camera_update(Camera _new_camera)
    {
        new_camera = _new_camera;
        _update_camera = true;
    }

    void stop_render_thread()
    {
        should_run = false;
        _surface_lock.notify_all();
        render_thread.join();
    }
   
};