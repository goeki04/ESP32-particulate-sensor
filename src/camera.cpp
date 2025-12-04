#include "pch.h"
#include "camera.h"

void Camera::cameraInput()
{
	Uint32 mouseButtonState = SDL_GetMouseState(NULL, NULL);
	if (mouseButtonState & SDL_BUTTON_MASK(SDL_BUTTON_LEFT)) {
		// 1. Hole die aktuelle absolute Mausposition
		float currentMouseX, currentMouseY;
		SDL_GetMouseState(&currentMouseX, &currentMouseY);
		if (lastMouseX == 0.0f && lastMouseY == 0.0f) {
			lastMouseX = (float)currentMouseX;
			lastMouseY = (float)currentMouseY;
		}
		float deltaX = (float)currentMouseX - lastMouseX;
		float deltaY = (float)currentMouseY - lastMouseY;
		angleX += deltaX * sensitivity;
		angleY -= deltaY * sensitivity; // Y-Achse meist invertiert

		// 4. Aktualisiere die letzte Position
		lastMouseX = (float)currentMouseX;
		lastMouseY = (float)currentMouseY;

		// 5. Aktualisiere die View Matrix
		m_ViewMatrix = calculateCameraOrbit();
	}
}

glm::mat4 Camera::calculateCameraOrbit()
{
	const float radius = 10.0f;
	angleY = glm::clamp(angleY, -glm::half_pi<float>() + 0.1f, glm::half_pi<float>() - 0.1f);

	float camX = sin(angleX) * cos(angleY) * radius;
	float camY = sin(angleY) * radius;
	float camZ = cos(angleX) * cos(angleY) * radius;

	cameraPos = glm::vec3(camX, camY, camZ);
	return glm::lookAt(cameraPos, glm::vec3(0.0f, 0.0f, 0.0f), up);
}
