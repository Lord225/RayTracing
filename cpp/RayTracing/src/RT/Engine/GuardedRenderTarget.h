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
        enum class ThreadState
        {
            RENDERING, // Render thread is writing to buffer 
            REQUEST,   // Window thread wants to read
            READING   // Render thread waits for window thread
        };

        std::vector<glm::vec3> surf;     // Resouce
        std::mutex surf_m;
        size_t img_w;


        ThreadState state = ThreadState::RENDERING;
        std::condition_variable _surface_lock;
        std::mutex state_m;
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
                    owner->state = ThreadState::RENDERING;
                    owner->_surface_lock.notify_all();
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
            std::unique_lock<std::mutex> lk_state(state_m);
            if (_surface_lock.wait_for(lk_state, std::chrono::milliseconds(2), [&]() { return state == ThreadState::READING; }))
            {
                // state == READING, we should be fine to lock on it.
                std::unique_lock<std::mutex> lk_surf(surf_m);
                return Surf(std::ref(surf), img_w, std::move(lk_surf), this);
            }
            else
            {
                if (state == ThreadState::RENDERING)
                {
                    state = ThreadState::REQUEST;
                    _surface_lock.notify_one();
                }
                else if (state == ThreadState::REQUEST)
                {
                    
                }
            }
            return {};
        }

        /// <summary>
        /// Function will aqure resources, but it has lower priority than request_asap_surface
        /// </summary>
        Surf request_surface()
        {
            std::unique_lock<std::mutex> state_lk(state_m);
            if (state == ThreadState::REQUEST)
            {
                state = ThreadState::READING;

                state_lk.unlock();

                _surface_lock.notify_all();
                
                std::unique_lock<std::mutex> state_lk(state_m);
                _surface_lock.wait(state_lk, [&]() {return state != ThreadState::RENDERING; });
                state_lk.unlock();
            }
            {
                std::unique_lock<std::mutex> surf_lk(surf_m);
                return Surf(std::ref(surf), img_w, std::move(surf_lk), nullptr);
            }
        }

        void stop()
        {
            _surface_lock.notify_all();
        }
    };
}
