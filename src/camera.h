#pragma once
class Camera {
	
	float mouseX = 0.0f;
	float mouseY = 0.0f;
	float angleX = -0.8f;   // ~45°
	float angleY = -0.5f;
	glm::vec3 target = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 cameraPos = glm::vec3(0.0f,0.0f,0.0f);
	float lastMouseX = 0.0f;
	float lastMouseY = 0.0f;
	const float sensitivity = 0.01f;
	float fov = 45.0f;
	float fovMin = 10.0f, fovMax = 50.0f;
	static glm::mat4 projection;
public:
	glm::mat4 m_ViewMatrix = calculateCameraOrbit();

	void cameraMovement();
	void zoom(SDL_Event* event);
    void setProjectionMatrix(float viewPortSizeX, float viewportSizeY);
    static glm::mat4 getProjectionMatrix();
	glm::mat4 calculateCameraOrbit();
};