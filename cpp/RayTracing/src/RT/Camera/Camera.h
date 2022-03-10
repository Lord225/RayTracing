#pragma once
#include <glm.hpp>
#include <iostream>
#include <gtc/matrix_transform.hpp>
#include "Ray.h"

namespace Cam
{
	class Camera
	{
	public:
		glm::mat4 inverse;
		glm::vec3 origin;
		Camera(const glm::vec3& pos, const glm::vec3& camera_dir, float aspectratio, float focal, float near = 0.2, float far = 1.0)
		{
			glm::mat4 proj = glm::perspective(focal, aspectratio, near, far);
			glm::mat4 view = glm::lookAt(glm::vec3(0.0f), camera_dir, { 0.0f, 1.0f, 0.0f });

			inverse = glm::inverse(proj * view);
			origin = pos;
		}

		Camera(const Camera&) = default;
		Camera& operator=(const Camera&) = default;

		ray genray(glm::vec2 uv) const
		{
			glm::vec4 screenPos = glm::vec4(uv, 1.0f, 1.0f);
			glm::vec4 worldPos = inverse * screenPos;

			glm::vec3 dir = glm::normalize(glm::vec3(worldPos));
			glm::vec3 orig = origin;
			return ray(orig, dir);
		}
	};
}

