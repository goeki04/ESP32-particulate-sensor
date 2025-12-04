#pragma once
class Camera {
	
	float mouseX = 0.0f;
	float mouseY = 0.0f;
	float angleX = 0;
	float angleY = 0;
	glm::vec3 target = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 cameraPos = glm::vec3(0.0f,0.0f,0.0f);
	float lastMouseX = 0.0f;
	float lastMouseY = 0.0f;
	const float sensitivity = 0.01f;
public:
	glm::mat4 m_ViewMatrix = calculateCameraOrbit();
	void cameraInput();
	glm::mat4 calculateCameraOrbit();
};