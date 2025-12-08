#include "pch.h"
#include "camera.h"
#include "GuiManager.h"
glm::mat4 Camera::projection = glm::mat4(1.0f);
void Camera::cameraMovement()
{
	Uint32 mouseButtonState = SDL_GetMouseState(NULL, NULL);
	if (mouseButtonState & SDL_BUTTON_MASK(SDL_BUTTON_LEFT)) {
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

		lastMouseX = (float)currentMouseX;
		lastMouseY = (float)currentMouseY;
		m_ViewMatrix = calculateCameraOrbit();
	}
}

void Camera::zoom(SDL_Event* event) {
	if (event->type == SDL_EVENT_MOUSE_WHEEL) {
		if (event->wheel.y > 0) {
			fov = std::clamp(fov + 1, fovMin, fovMax);
		}
		else if (event->wheel.y < 0) {
			fov = std::clamp(fov-1,fovMin,fovMax);
		}
	}
}

void Camera::setProjectionMatrix(float viewportSizeX, float viewportSizeY) {
	projection = glm::perspective(glm::radians(fov), viewportSizeX / viewportSizeY, 0.1f, 100.0f);
}

glm::mat4 Camera::getProjectionMatrix() {
	return projection;
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
