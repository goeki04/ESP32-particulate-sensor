#pragma once
#include "a_primitives.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
#include <type_traits>
namespace Andromeda::amath {
	struct Ray {
		vec3 origin;
		vec3 direction;
	};

	struct CameraData {
		vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
		vec3 cameraPos = glm::vec3(0.0f, 5.0f, 0.0f);
		vec3 cameraTarget = glm::vec3(0.0f);
		float fov = 45.0f;
		float fovMin = 10.0f, fovMax = 50.0f;
		mat4 projection = { 1.0f };
		bool canRotate = false;
		float lastMouseY = 0.0f;
		float lastMouseX = 0.0f;
		float yaw = -90.0f;
		float pitch = 0.0f;
		float sensitivity = 0.1f;
		vec3 target = vec3(0.0f);
		vec3 forward = vec3(0.0f);
		vec3 right = vec3(0.0f);
		float speed = 25.0f;
		mat4 viewMatrix = { 1.0f };
		bool hasValidPickRay = false;
		vec2 framebufferSize = vec2(0.0f);
		vec2 viewportSize = vec2(0.0f);
		vec2 viewportPos = vec2(0.0f);
		float imGuiMouseX = 0.0f;
		float imGuiMouseY = 0.0f;
		Ray cursorToWorldRay{ vec3(0.0f), vec3(0.0f) };
	};

	template<typename T>
	[[nodiscard]] constexpr inline T inverse(const T& matrix) noexcept {
		return glm::inverse(matrix);
	}

	template<typename T>
	[[nodiscard]] constexpr inline T radians(const T degrees) noexcept {
		return glm::radians(degrees);
	}
	template<typename T>
	[[nodiscard]] constexpr inline T normalize(const T& value) noexcept {
		return glm::normalize(value);
	}
	template<typename T>
	[[nodiscard]] constexpr inline T cross(const T& x, const T& y) noexcept {
		return glm::cross(x,y);
	}

	template<typename T>
	[[nodiscard]] inline mat4 perspective(T fov, T aspect, T zNear, T zFar) noexcept {
		return glm::perspective(fov, aspect, zNear, zFar);
	}
	[[nodiscard]] inline mat4 lookAt(const vec3& eye, const vec3& center, const vec3& up) noexcept {
		return glm::lookAt(eye, center, up);
	}

	template<typename T>
	[[nodiscard]] constexpr inline T abs(T value) noexcept {
		return glm::abs(value);
	}

	template<typename T>
	[[nodiscard]] inline std::string to_string(const T& value) {
		if constexpr(std::is_arithmetic_v<T>) {
			return std::to_string(value);
		}
		else {
			return glm::to_string(value);
		}
	}

	inline bool RayIntersectsXZPlane(const amath::Ray& ray, float planeY, vec3& outHitPoint)
	{
		if (amath::abs(ray.direction.y) < 1e-6f) {
			return false;
		}
		float t = (planeY - ray.origin.y) / ray.direction.y;
		if (t < 0.0f) {
			return false;
		}
		outHitPoint = ray.origin + t * ray.direction;
		return true;
	}
}