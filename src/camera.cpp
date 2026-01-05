#include "pch.h"
#include "camera.h"
#include "GuiManager.h"
#include "WindowManager.h"
glm::mat4 Camera::projection = glm::mat4(1.0f);
void Camera::cursorToWorldPos()
{
    
    float x = (2.0f * mouseX) / Window::g_WindowWidth - 1.0f;
    float y = 1.0f - (2.0f * mouseY) / Window::g_WindowHeight; // Y-Achse invertieren!
}
void Camera::cameraMovement()
{
    Uint32 mouseButtonState = SDL_GetMouseState(NULL, NULL);
    bool mouseDown = mouseButtonState & SDL_BUTTON_MASK(SDL_BUTTON_RIGHT);

    float currentMouseX, currentMouseY;
    SDL_GetMouseState(&currentMouseX, &currentMouseY);

    if (mouseDown)
    {
        if (!mouseDownLastFrame)
        {
            lastMouseX = currentMouseX;
            lastMouseY = currentMouseY;
        }
        float deltaX = currentMouseX - lastMouseX;
        float deltaY = currentMouseY - lastMouseY;

        angleX += deltaX * sensitivity;
        angleY -= deltaY * sensitivity;

        lastMouseX = currentMouseX;
        lastMouseY = currentMouseY;

        m_ViewMatrix = calculateCameraOrbit();
    }
    mouseDownLastFrame = mouseDown;
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
