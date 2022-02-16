#pragma once
#include <SDL/SDL.h>
#include <glm.hpp>
#include "asserts.h"
#include <iostream>

class SurfaceWrapper
{
	SDL_Surface* surf = nullptr;
public:
	enum class State {
		ERROR = 0,
		GOOD = 1,
	};

	State setTarget(SDL_Surface* _surf)
	{
		if (_surf->format->BitsPerPixel != 32)
		{
			SDL_SetError("Surface pixel format should be 32 bit");
			return State::ERROR;
		}
		surf = _surf;
		return State::GOOD;
	}

	struct Pixel
	{
		uint8_t b;
		uint8_t g;
		uint8_t r;
		uint8_t a;
	};

	static_assert(sizeof(Pixel) == 4, "Cannot pack 'pixel' struct into one 32bit value.");

	void setPixel(size_t x, size_t y, const glm::vec3& color)
	{
		const auto pixels = reinterpret_cast<Pixel*>(surf->pixels);
		const auto _w = w();
		const auto _h = h();

#ifdef _DEBUG
		assert_less(x, _w);
		assert_less(y, _h);
#endif
		pixels[x + y * _w].r = static_cast<int>(255.999 * color.r);
		pixels[x + y * _w].g = static_cast<int>(255.999 * color.g);
		pixels[x + y * _w].b = static_cast<int>(255.999 * color.b);

	}
	int w()
	{
		return surf->w;
	}
	int h()
	{
		return surf->h;
	}
};

