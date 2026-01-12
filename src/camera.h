#pragma once
struct AABB;

struct Ray {
	glm::vec3 origin;
	glm::vec3 direction;
};

class Camera {
private:
	bool mouseRightDownLastFrame = false;
	float m_MouseX = 0.0f;
	float m_MouseY = 0.0f;
	float m_AngleX = -0.8f;
	float m_AngleY = -0.5f;
	glm::vec3 m_Target = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 m_Up = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 m_CameraPos = glm::vec3(0.0f,0.0f,0.0f);
	float m_LastMouseX = 0.0f;
	float m_LastMouseY = 0.0f;
	const float m_Sensitivity = 0.01f;
	float m_Fov = 45.0f;
	float m_FovMin = 10.0f, m_FovMax = 50.0f;
	static glm::mat4 m_Projection;

public:
	bool m_HasValidPickRay;
	glm::vec2 m_framebufferSize;
	glm::vec2 m_ViewportSize;
	glm::vec2 m_ViewportPos;
	float m_ImGuiMouseX;
	float m_ImGuiMouseY;
	bool m_CursorToWorldRayEnabled = false;
	glm::mat4 m_ViewMatrix = calculateCameraOrbit();
	Ray m_CursorToWorldRay;
	bool RayIntersectsXZPlane(const Ray& ray, float planeY, glm::vec3& outHitPoint);
	void cameraMovement();
	void updatePickingRay();
	void zoom(SDL_Event* event);
    void setProjectionMatrix(float viewPortSizeX, float viewportSizeY);
    static glm::mat4 getProjectionMatrix();
	glm::mat4 calculateCameraOrbit();
	Ray cursorToWorldRay();
private:

};