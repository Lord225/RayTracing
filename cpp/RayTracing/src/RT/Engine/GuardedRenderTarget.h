#pragma once

#include <thread>
#include <glm.hpp>
#include <mutex>
#include <vector>
#include <optional>

namespace RT
{
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
            Surf(std::reference_wrapper<std::vector<glm::vec3>> surf, size_t img_w, std::unique_lock<std::mutex>&& lk, GuardedRenderTarget* owner)
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
}
