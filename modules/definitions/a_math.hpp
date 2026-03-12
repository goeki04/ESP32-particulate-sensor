#pragma once
#include "a_primitives.hpp"
namespace Andromeda::amath {
	struct Ray {
		vec3 origin;
		vec3 direction;
	};

	struct CameraData {
		vec3 m_Up = glm::vec3(0.0f, 1.0f, 0.0f);
		vec3 m_CameraPos = glm::vec3(0.0f, 5.0f, 0.0f);
		vec3 m_CameraTarget = glm::vec3(0.0f);
		float m_Fov = 45.0f;
		float m_FovMin = 10.0f, m_FovMax = 50.0f;
		mat4 m_Projection = { 1.0f };

		float m_LastMouseY = 0.0f;
		float m_LastMouseX = 0.0f;
		float m_Yaw = -90.0f;
		float m_Pitch = 0.0f;
		float m_Sensitivity = 0.1f;
		vec3 m_Target;
		vec3 m_Forward;
		vec3 m_Right;
		float m_Speed = 25.0f;
		mat4 m_ViewMatrix = { 1.0f };
		bool m_HasValidPickRay;
		vec2 m_framebufferSize;
		vec2 m_ViewportSize;
		vec2 m_ViewportPos;
		float m_ImGuiMouseX;
		float m_ImGuiMouseY;
		Ray m_CursorToWorldRay{ vec3(0.0f), vec3(0.0f) };
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
}