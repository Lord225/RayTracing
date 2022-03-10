#pragma once
#include <vector>
#include <memory>
#include "Hittable.h"


namespace Primitives
{
	class HitVector : public IHittable, public std::vector<std::unique_ptr<IHittable>>
	{
	public:
		std::optional<Record> intersect(const ray& ray, float min, float max) const override
		{
			float t = max;
			std::optional<Record> hit = std::nullopt;

			for (const auto& obj : *this)
			{
				if (auto result = obj->intersect(ray, min, t))
				{
					t = (*result).dis;
					hit = result;
				}
			}

			return hit;
		}
	};
}