#include <iostream>
#include <glm.hpp>
#include <SDL.h>
#include <memory>
#include "Utils/SurfaceWrapper.h"

#include "RT/Camera/Camera.h"
#include "RT/Camera/Ray.h"

#include "RT/Primitives/Sphere.h"
#include "RT/Primitives/HitVector.h"

#include "RT/Engine/GuardedRenderTarget.h"
#include "RT/Engine/RayTracer.h"

#include "RT/Material/Material.h"

#include <random>

int SDL_Error_Handle(std::string message = "[ERROR]:")
{
    const char* err = SDL_GetError();
#ifdef _DEBUG
    if (strlen(err) == 0)
        std::cout << "[WARN]: " << " SDL_Error_Handle callend without SDL_GetError error" << std::endl;
#endif
    std::cerr << message << " " << err << std::endl;
    return -1;
}


int main(int argc, char* argv[])
{
    if(SDL_Init(SDL_INIT_EVERYTHING) != 0)
        return SDL_Error_Handle();
    
    const auto screen_w = 1280;
    const auto screen_h = 720;
    const auto movement_speed = 0.005f;
    const auto rotation_speed = 0.1f;

    SDL_Window* window = SDL_CreateWindow("RT Demo (C++)", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screen_w, screen_h, SDL_WINDOW_SHOWN);

    if (window == nullptr)
        return SDL_Error_Handle();

    SDL_Surface* surf = SDL_GetWindowSurface(window);

    if (surf == nullptr)
        return SDL_Error_Handle();

    Utils::SurfaceWrapper pixels;
    if(pixels.setTarget(surf) == Utils::SurfaceWrapper::State::ERROR)
        return SDL_Error_Handle();

    SDL_CaptureMouse(SDL_bool(true));

    const float aspectratio = ((float)pixels.h()) / pixels.w();
    const float fl = 1.3f;

    glm::vec3 camerapos = { -2.0f, 0.0f, 0.0f };
    glm::vec2 yawpich = { 0.0f, 0.0f };
    glm::vec3 dir_front, dir_right, dir_up;

    Cam::Camera camera(camerapos, { 1.0f, 0.0f, 0.0f }, aspectratio, fl);

    using namespace Primitives;
    using namespace Mat;
    HitVector world;

    world.push_back(std::make_unique<Sphere>(glm::vec3(0.0f,  0.0f, 0.0f),   0.5f,    std::make_shared<Diffuse>(glm::vec3(0.7f, 0.3f, 0.3f))));
    world.push_back(std::make_unique<Sphere>(glm::vec3(0.0f,  0.0f, 100.5f), 100.0f,  std::make_shared<Diffuse>(glm::vec3(0.21, 0.37, 0.69))));
    world.push_back(std::make_unique<Sphere>(glm::vec3(0.0f, -1.0f, 0.2f),   0.3f,    std::make_shared<Metalic>(glm::vec3(0.8f, 0.8f, 0.8f), 0.0f)));
    world.push_back(std::make_unique<Sphere>(glm::vec3(0.0f,  1.0f, 0.0f),   0.4f,    std::make_shared<Refract>(10.0f)));

    RT::GuardedRenderTarget target(std::vector<glm::vec3>(pixels.w()*pixels.h()), pixels.w());

    RayTracer engine(pixels, target, world, camera, 32, 5, 32);
    engine.request_camera_update(camera);

    using namespace std::chrono;

    bool running = true;
    bool need_rerender = false;
    bool is_done_lock = false;
    

    long int frame_counter = 0;
    SDL_Event ev;
    Uint64 NOW = SDL_GetPerformanceCounter();
    Uint64 LAST = 0;
    int lastx = 0, lasty = 0;
    float deltaTime = 0;
    bool key_pressed[322]{};

    while (running)
    {
        LAST = NOW;
        NOW = SDL_GetPerformanceCounter();
        deltaTime = (float)((NOW - LAST) * 1000 / (float)SDL_GetPerformanceFrequency());

        frame_counter += 1;
        while (SDL_PollEvent(&ev) != 0)
        {
            switch (ev.type) {
            case SDL_QUIT:
                running = false;
                break;
            case SDL_KEYDOWN:
                if(ev.key.keysym.sym < 322)
                    key_pressed[ev.key.keysym.sym] = true;
                break;
            case SDL_KEYUP:
                if (ev.key.keysym.sym < 322)
                    key_pressed[ev.key.keysym.sym] = false;
                break;
            default:
                break;
            }
        }

        int currentx, currenty;
        auto state = SDL_GetGlobalMouseState(&currentx, &currenty);
        int relx = lastx - currentx, rely = lasty - currenty;
        lastx = currentx;
        lasty = currenty;
        if (state == 0)
        {
            relx = 0;
            rely = 0;
        }

        if(relx != 0 || rely != 0)
            need_rerender = false;
       
        if (key_pressed[SDLK_w])
        {
            camerapos += dir_front * deltaTime * movement_speed;
            need_rerender = true;
        }
        if (key_pressed[SDLK_s])
        {
            camerapos -= dir_front * deltaTime * movement_speed;
            need_rerender = true;
        }
        if (key_pressed[SDLK_a])
        {
            camerapos += dir_right * deltaTime * movement_speed;
            need_rerender = true;
        }
        if (key_pressed[SDLK_d])
        {
            camerapos -= dir_right * deltaTime * movement_speed;
            need_rerender = true;
        }
        if (key_pressed[SDLK_z])
        {
            camerapos -= dir_up * deltaTime * movement_speed;
            need_rerender = true;
        }
        if (key_pressed[SDLK_SPACE])
        {
            camerapos += dir_up * deltaTime * movement_speed;
            need_rerender = true;
        }
        if (key_pressed[SDLK_ESCAPE])
        {
            running = false;
        }
        
        engine.request_surface_update();

        if (need_rerender)
        {
            yawpich += glm::vec2(-relx * rotation_speed, rely * rotation_speed);
            yawpich.y = std::clamp(yawpich.y, -89.0f, 89.0f);
            
            dir_front.x = cos(glm::radians(yawpich.y)) * cos(glm::radians(yawpich.x));
            dir_front.y = sin(glm::radians(yawpich.x));
            dir_front.z = sin(glm::radians(yawpich.y)) * cos(glm::radians(yawpich.x));
            
            dir_front = glm::normalize(dir_front);
            dir_right = glm::normalize(glm::cross(dir_front, {0.0f, 0.0f, 1.0f}));
            dir_up = glm::normalize(glm::cross(dir_right, dir_front));

            camera = Cam::Camera(camerapos, dir_front, aspectratio, fl);

            engine.request_camera_update(camera);

            need_rerender = false;
            is_done_lock = false;
        }

        if (frame_counter % 10 == 0 && !engine.renderer.is_done())
        {
            std::cout << "Time per iteration: " << engine.renderer.get_time_per_sample().count() << "ms\n";
        }

        if (engine.renderer.is_done() && is_done_lock == false)
        {
            is_done_lock = true;
            std::cout << "Time: " << engine.renderer.get_total_render_time().count()/1000.0f << "s\n";
        }
        

        SDL_UpdateWindowSurface(window);
    }
    engine.kill_render_thread();

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
