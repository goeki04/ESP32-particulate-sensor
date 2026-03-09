#pragma once
struct AABB;
#include "a_math.hpp"
namespace Andromeda  {

	class Camera {
	private:
		glm::vec3 m_Up = glm::vec3(0.0f, 1.0f, 0.0f);
		glm::vec3 m_CameraPos = glm::vec3(0.0f, 5.0f, 0.0f);
		glm::vec3 m_CameraTarget = glm::vec3(0.0f);
		float m_Fov = 45.0f;
		float m_FovMin = 10.0f, m_FovMax = 50.0f;
		static glm::mat4 m_Projection;

		float m_LastMouseY = 0.0f;
		float m_LastMouseX = 0.0f;
		float m_Yaw = -90.0f;
		float m_Pitch = 0.0f;
		float m_Sensitivity = 0.1f;
		glm::vec3 m_Target;
		glm::vec3 m_Forward;
		glm::vec3 m_Right;
		float m_Speed = 25.0f;
		glm::mat4 m_ViewMatrix = calculateCameraOrbit();
	public:

		bool m_HasValidPickRay;
		glm::vec2 m_framebufferSize;
		glm::vec2 m_ViewportSize;
		glm::vec2 m_ViewportPos;
		float m_ImGuiMouseX;
		float m_ImGuiMouseY;

		Ray m_CursorToWorldRay{ glm::vec3(0.0f),glm::vec3(0.0f) };
		bool RayIntersectsXZPlane(const Ray& ray, float planeY, glm::vec3& outHitPoint);
		void cameraMovement();
		glm::vec3 getCameraPos() const;
		glm::mat4 getViewMatrix() const;
		void updatePickingRay();
		void zoom(SDL_Event* event);
		void setProjectionMatrix(float viewPortSizeX, float viewportSizeY) const;
		static glm::mat4 getProjectionMatrix();
		glm::mat4 calculateCameraOrbit();
		Ray cursorToWorldRay() const;
	};
}