#include "pch.h"
#include "camera.h"
#include "GuiManager.h"
#include "WindowManager.h"
glm::mat4 Camera::m_Projection = glm::mat4(1.0f);
Ray Camera::cursorToWorldRay()
{
    float x = (2.0f * m_ImGuiMouseX) / m_framebufferSize.x - 1.0f;
    float y = 1.0f - (2.0f * m_ImGuiMouseY) / m_framebufferSize.y;
    glm::vec4 rayClip(x, y, -1.0f, 1.0f);

    glm::vec4 rayView = glm::inverse(m_Projection) * rayClip;
    rayView = glm::vec4(rayView.x, rayView.y, -1.0f, 0.0f);
    glm::vec4 rayDir4 = glm::inverse(m_ViewMatrix) * rayView;
    Ray ray;

    ray.direction = glm::normalize(glm::vec3(rayDir4));
    ray.origin = m_CameraPos;
    return ray;
}

bool Camera::RayIntersectsXZPlane(const Ray& ray, float planeY,glm::vec3& outHitPoint)
{
    if (glm::abs(ray.direction.y) < 1e-6f) {
        return false;
    }

    float t = (planeY - ray.origin.y) / ray.direction.y;

    if (t < 0.0f) {
        return false;
    }

    outHitPoint = ray.origin + t * ray.direction;
    return true;
}
void Camera::cameraMovement()
{
    Uint32 mouseButtonState = SDL_GetMouseState(NULL, NULL);
    bool mouseRightDown = mouseButtonState & SDL_BUTTON_MASK(SDL_BUTTON_RIGHT);
    bool mouseLeftDown = mouseButtonState & SDL_BUTTON_MASK(SDL_BUTTON_LEFT);
    float currentMouseX, currentMouseY;
    SDL_GetMouseState(&currentMouseX, &currentMouseY);

    if (mouseRightDown)
    {
        if (!mouseRightDownLastFrame)
        {
            m_LastMouseX = currentMouseX;
            m_LastMouseY = currentMouseY;
        }
        float deltaX = currentMouseX - m_LastMouseX;
        float deltaY = currentMouseY - m_LastMouseY;

        m_AngleX += deltaX * m_Sensitivity;
        m_AngleY -= deltaY * m_Sensitivity;

        m_LastMouseX = currentMouseX;
        m_LastMouseY = currentMouseY;

        m_ViewMatrix = calculateCameraOrbit();
    }
    mouseRightDownLastFrame = mouseRightDown;
    m_CursorToWorldRay = cursorToWorldRay();
}

void Camera::updatePickingRay()
{
    ImVec2 mouse = ImGui::GetMousePos();

    float localX = mouse.x - m_ViewportPos.x;
    float localY = mouse.y - m_ViewportPos.y;

    if (localX < 0 || localY < 0 || localX >= m_ViewportSize.x || localY >= m_ViewportSize.y) {
        m_HasValidPickRay = false;
        return;
    }

    m_HasValidPickRay = true;
    m_ImGuiMouseX = localX;
    m_ImGuiMouseY = localY;
    m_framebufferSize = glm::vec2(m_ViewportSize.x, m_ViewportSize.y);

    m_CursorToWorldRay = cursorToWorldRay();
}

void Camera::zoom(SDL_Event* event) {
	if (event->type == SDL_EVENT_MOUSE_WHEEL) {
		if (event->wheel.y > 0) {
			m_Fov = std::clamp(m_Fov + 1, m_FovMin, m_FovMax);
		}
		else if (event->wheel.y < 0) {
			m_Fov = std::clamp(m_Fov-1,m_FovMin,m_FovMax);
		}
	}
}

void Camera::setProjectionMatrix(float viewportSizeX, float viewportSizeY) {
	m_Projection = glm::perspective(glm::radians(m_Fov), viewportSizeX / viewportSizeY, 0.1f, 100.0f);
}

glm::mat4 Camera::getProjectionMatrix() {
	return m_Projection;
}

glm::mat4 Camera::calculateCameraOrbit()
{
	const float radius = 10.0f;
	m_AngleY = glm::clamp(m_AngleY, -glm::half_pi<float>() + 0.1f, glm::half_pi<float>() - 0.1f);

	float camX = sin(m_AngleX) * cos(m_AngleY) * radius;
	float camY = sin(m_AngleY) * radius;
	float camZ = cos(m_AngleX) * cos(m_AngleY) * radius;

	m_CameraPos = glm::vec3(camX, camY, camZ);
	return glm::lookAt(m_CameraPos, glm::vec3(0.0f, 0.0f, 0.0f), m_Up);
}
