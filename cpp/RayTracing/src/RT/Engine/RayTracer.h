#pragma once
#include "../../Utils/SurfaceWrapper.h"
#include "../Camera/Camera.h"
#include "../Camera/Ray.h"
#include "../Primitives/Hittable.h"
#include "../../Utils/VecStuff.h"
#include "../../Utils/thread_pool.hpp"
#include <glm.hpp>
#include <optional>
#include <tuple>
#include <iostream>
#include <vector>
#include <random>
#include "RTRenderer.h"


class RayTracer {
    Cam::Camera camera;
    Primitives::IHittable& world;
    Utils::SurfaceWrapper& pixels;
    
    RT::GuardedRenderTarget& image;

    void update_surface(RT::GuardedRenderTarget::Surf& surf)
    {
        float ratio = renderer.iterations();
        for (int j = 0; j < pixels.h(); ++j)
        {
            for (int i = 0; i < pixels.w(); ++i)
            {
                pixels.setPixel(i, j, surf.get_pixel(i, j) / ratio);
            }
        }
    }
public:

    RT::RTRenderer renderer;

    RayTracer(Utils::SurfaceWrapper& pixels, RT::GuardedRenderTarget& render_target, Primitives::IHittable& world, Cam::Camera camera, int _max_iters, int _max_bounces, int _max_workers = 8) :
        camera(camera),
        world(world),
        pixels(pixels),
        renderer
        (
            render_target,
            _max_iters,
            _max_bounces,
            _max_workers
        ),
        image(render_target)
    {
        renderer.request_world_update(world);
        renderer.request_camera_update(camera);
    }

    void request_surface_update()
    {
        //std::cout << "(window) Trying Locking" << std::endl;
        std::optional<RT::GuardedRenderTarget::Surf> resource = image.request_asap_surface();

        if (resource)
        {
            //std::cout << "(window) Locked " << std::endl;
            auto surf = std::move(resource.value());

            update_surface(surf);
        }
        else
        {
            //std::cout << "(window) Failed" << std::endl;
        }

    }

    void request_camera_update(Cam::Camera new_cam)
    {
        renderer.request_camera_update(new_cam);
    }


    void kill_render_thread()
    {
        renderer.kill_render_thread();
    }
   
};