#pragma once
#include <thread>
#include <glm.hpp>
#include "../Utils/thread_pool.hpp"
#include "Camera.h"
#include "Ray.h"
#include "Primitives/Hittable.h"
#include "shade.h"


namespace RT
{
    struct World
    {
        std::optional<Camera> camera;
        std::optional<std::reference_wrapper<IHittable>> world;
    };

    class GuardedRenderTarget
    {
        std::vector<glm::vec3> surf;     // Resouce
        size_t img_w;

        std::mutex m;
        
        std::atomic_bool _ready = false;
        std::atomic_bool _update_surface;

        std::condition_variable _surface_lock;
    public:
        struct Surf
        {
            std::vector<glm::vec3>& raw;
            size_t img_w;
            Surf(std::reference_wrapper<std::vector<glm::vec3>> surf, size_t img_w, std::unique_lock<std::mutex>&& lk, GuardedRenderTarget * owner) 
                : raw(surf), img_w(img_w), surf_lock(std::move(lk)), owner(owner) {}
            
            Surf(Surf&&) = default;
            Surf& operator=(Surf&&) = default;
            ~Surf()
            {
                if (owner != nullptr)
                {
                    owner->_surface_lock.notify_all();
                    owner->_ready = false;
                }
            }

            glm::vec3& get_pixel(size_t x, size_t y)
            {
                return raw[x + img_w * y];
            }

            size_t w() { return img_w; }
            size_t h() { return raw.size() / img_w; }
        private:
            std::unique_lock<std::mutex> surf_lock; // I swear `raw` is yours only.
            GuardedRenderTarget* owner;
        };

        GuardedRenderTarget(std::vector<glm::vec3>&& surf, size_t img_w) : surf(std::move(surf)), img_w(img_w) {}

        /// <summary>
        /// Function will not aquire resources immidetly, 
        /// but all other threads will not be able to lock on resources if this function was called
        /// </summary>
        std::optional<Surf> request_asap_surface()
        {
            std::unique_lock<std::mutex> lk(m);
            if (_surface_lock.wait_for(lk, std::chrono::milliseconds(10), [&]() { return _ready.load(); }))
            {
                return Surf(std::ref(surf), img_w, std::move(lk), this);
            }
            else
            {
                _update_surface = true;
                return {}; // Return None, but requests surf_update
            }
        }

        /// <summary>
        /// Function will aqure resources, but it has lower priority than request_asap_surface
        /// </summary>
        Surf request_surface()
        {
            std::unique_lock<std::mutex> lk(m);
            if (_update_surface)
            {
                _update_surface = false;
                _surface_lock.notify_one();
                _ready.store(true);
                _surface_lock.wait(lk); // waits for high priority thread
            }
            return Surf(std::ref(surf), img_w, std::move(lk), nullptr);
        }

        void stop()
        {
            _surface_lock.notify_all();
        }
    };

    using real_milliseconds = std::chrono::duration<double, std::ratio<1, 1000>>;

    class RTRenderer
    {
    private:
        // render thread handle ( render_loop function )
        std::thread render_thread;
        // max workers in render workers pool
        int _max_workers;

        // arleady done iterations 
        int _iterations;

        int _max_iterations;
        int _max_bounces;

        // worker thread will render to this texture
        GuardedRenderTarget& render_target;

        // RND for rendering
        std::uniform_real_distribution<float> uniform;
        std::mt19937 random;
        
        real_milliseconds _total_render_time;
           
        // Flag for updateing camera
        std::atomic_bool _update_camera;
        std::optional<Camera> new_camera;
    public:
        bool should_run;
        World renderable_world;

        RTRenderer(GuardedRenderTarget& image, int max_iters, int _max_bounces, int _max_workers) :
            render_target(image),
            uniform(0.0f, 1.0f),
            _total_render_time(0),
            _iterations(0),
            _max_bounces(_max_bounces),
            _max_iterations(max_iters),
            _max_workers(_max_workers)
        {
            should_run = true;
            render_thread = std::thread([&]() { render_loop(); });
        }

        void request_camera_update(Camera _new_camera)
        {
            new_camera = _new_camera;
            _update_camera = true;
        }

        void request_world_update(IHittable& world)
        {
            renderable_world.world = world;
        }

        void kill_render_thread()
        {
            should_run = false;
            render_target.stop();
            render_thread.join();
        }

        int iterations()
        {
            return _iterations;
        }

        RT::real_milliseconds get_total_render_time()
        {
            return _total_render_time;
        }
        RT::real_milliseconds get_time_per_sample()
        {
            return get_total_render_time() / (iterations() + 1);
        }
        bool is_done()
        {
            return _iterations >= _max_iterations;
        }

    private:

        std::pair<float, float> get_uv(float x, float y, std::pair<size_t, size_t> wh)
        {
            const float u = ((y + uniform(random)) / (wh.second - 1) - 0.5f) * 2.0f;
            const float v = ((x + uniform(random)) / (wh.first - 1) - 0.5f) * 2.0f;

            return { u, v };
        }

        void trace_indexes(GuardedRenderTarget::Surf& surf, int samples, int _bounces, const Camera& camera, const IHittable& world, int from, int to)
        {
            for (size_t i = from; i < to; i++)
            {
                const int x = i % surf.w();
                const int y = i / surf.w();

                glm::vec3 color = { 0.0f, 0.0f, 0.0f };

                for (size_t sample = 0; sample < samples; sample++)
                {
                    const auto [u, v] = get_uv(float(x), float(y), std::make_pair(surf.w(), surf.h()));

                    const auto r = camera.genray({ u, v });

                    color += gen_color(r, world, random, _max_bounces);
                }
                surf.get_pixel(x, y) += color;
            }
        }

        void render_loop()
        {
            using namespace std::chrono;
            thread_pool pool(_max_workers);
            //std::cout << "(render) Start" << std::endl;
            while (should_run)
            {
                if (_iterations <= _max_iterations && (renderable_world.camera.has_value() && renderable_world.world.has_value()))
                {
                    const auto& camera = renderable_world.camera.value();
                    const auto& world = renderable_world.world.value();

                    {
                        //std::cout << "(render) Locking " << std::endl;
                        GuardedRenderTarget::Surf recources = render_target.request_surface();   // Lock on render_target
                        //std::cout << "(render) Locked" << std::endl;
                        auto start = high_resolution_clock::now();

                        pool.parallelize_loop(0, recources.raw.size(), [&](const int& a, const int& b) { trace_indexes(recources, 1, _max_bounces, camera, world, a, b); });
                        pool.wait_for_tasks();

                        auto end = high_resolution_clock::now();

                        _total_render_time += duration_cast<real_milliseconds>(end - start);
                        _iterations += 1;

                        //std::cout << "(render) Unlocking" << std::endl;
                        // render_target unlockned.
                    }
                }
                else
                {
                    std::this_thread::yield();
                }

                if (_update_camera)
                {
                    {
                        //std::cout << "(render) Locking Camera" << std::endl;
                        GuardedRenderTarget::Surf recources = render_target.request_surface(); // Lock on render_target
                        reset_render_target(recources);
                        // render_target unlockned.
                    }
                    //std::cout << "(render) Unlocking Camera" << std::endl;

                    if (new_camera.has_value()) {
                        renderable_world.camera = new_camera.value();
                        new_camera = std::nullopt;
                    }
                    _update_camera = false;
                }
            }
        }

        void reset_render_target(GuardedRenderTarget::Surf& surf)
        {
            for (auto& px : surf.raw)
            {
                px = { 0.0f, 0.0f, 0.0f };
            }
            _iterations = 0;
            _total_render_time = RT::real_milliseconds(0);
        }
    };

}