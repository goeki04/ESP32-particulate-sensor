#include "pch.h"
#include "camera.h"
#include "WindowManager.h"
#include "GuiRenderer.h"
#include "SubsystemManager.h"
namespace Andromeda {
    glm::mat4 Camera::m_Projection = glm::mat4(1.0f);
    Ray Camera::cursorToWorldRay() const
    {
        float x = (2.0f * m_ImGuiMouseX) / m_framebufferSize.x - 1.0f;
        float y = 1.0f - (2.0f * m_ImGuiMouseY) / m_framebufferSize.y;
        glm::vec4 rayClip(x, y, -1.0f, 1.0f);

        glm::vec4 rayView = glm::inverse(m_Projection) * rayClip;
        rayView = glm::vec4(rayView.x, rayView.y, -1.0f, 0.0f);
        glm::vec4 rayDir4 = glm::inverse(m_ViewMatrix) * rayView;
        Ray ray{ glm::vec3(0.0f),glm::vec3(0.0f) };

        ray.direction = glm::normalize(glm::vec3(rayDir4));
        ray.origin = m_CameraPos;
        return ray;
    }
    bool Camera::RayIntersectsXZPlane(const Ray& ray, float planeY, glm::vec3& outHitPoint)
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
        SDL_Window* currentWindow = SDL_GL_GetCurrentWindow();
        if (!Gui::GuiRenderer::s_ViewportFocused) {
            SDL_SetWindowRelativeMouseMode(SDL_GL_GetCurrentWindow(), false);
            return;
        }
        Uint32 mouseState = SDL_GetMouseState(NULL, NULL);
        bool rightMouseDown = mouseState & SDL_BUTTON_MASK(SDL_BUTTON_RIGHT);
        static bool wasRightMouseDown = false;
        if (rightMouseDown && !wasRightMouseDown) {
            ImGuiIO& io = ImGui::GetIO(); (void)io;
            io.ConfigFlags |= ImGuiConfigFlags_NoMouse;
            SDL_GetMouseState(&m_LastMouseX, &m_LastMouseY);
            SDL_SetWindowRelativeMouseMode(SDL_GL_GetCurrentWindow(), true);

            SDL_GetRelativeMouseState(NULL, NULL);
        }
        else if (!rightMouseDown && wasRightMouseDown) {
            ImGuiIO& io = ImGui::GetIO(); (void)io;
            io.ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
            SDL_WarpMouseInWindow(currentWindow, m_LastMouseX, m_LastMouseY);
            SDL_SetWindowRelativeMouseMode(SDL_GL_GetCurrentWindow(), false);
        }
        wasRightMouseDown = rightMouseDown;

        if (rightMouseDown) {
            float relX, relY;
            SDL_GetRelativeMouseState(&relX, &relY);

            m_Yaw += relX * m_Sensitivity;
            m_Pitch -= relY * m_Sensitivity;
            m_Pitch = std::clamp(m_Pitch, -89.0f, 89.0f);
        }

        float radYaw = glm::radians(m_Yaw);
        float radPitch = glm::radians(m_Pitch);

        m_Forward.x = cos(radYaw) * cos(radPitch);
        m_Forward.y = sin(radPitch);
        m_Forward.z = sin(radYaw) * cos(radPitch);
        m_Forward = glm::normalize(m_Forward);

        m_Right = glm::normalize(glm::cross(m_Forward, glm::vec3(0.0f, 1.0f, 0.0f)));

        const bool* keyboard = SDL_GetKeyboardState(NULL);
        glm::vec3 moveDir(0.0f);

        if (keyboard[SDL_SCANCODE_W]) moveDir += m_Forward;
        if (keyboard[SDL_SCANCODE_S]) moveDir -= m_Forward;
        if (keyboard[SDL_SCANCODE_A]) moveDir -= m_Right;
        if (keyboard[SDL_SCANCODE_D]) moveDir += m_Right;

        if (glm::length(moveDir) > 0.0f) {
            moveDir = glm::normalize(moveDir);
            m_CameraPos += moveDir * m_Speed * SystemManager::s_deltaTime;
        }

        m_Target = m_CameraPos + m_Forward;
        m_ViewMatrix = glm::lookAt(m_CameraPos, m_Target, glm::vec3(0.0f, 1.0f, 0.0f));
    }

    glm::vec3 Camera::getCameraPos() const
    {
        return m_CameraPos;
    }

    glm::mat4 Camera::getViewMatrix() const
    {
        return m_ViewMatrix;
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
                m_Fov = std::clamp(m_Fov - 1, m_FovMin, m_FovMax);
            }
        }
        setProjectionMatrix(m_ViewportSize.x, m_ViewportSize.y);
    }

    void Camera::setProjectionMatrix(float viewportSizeX, float viewportSizeY) const {
        float aspect = (viewportSizeY > 0) ? (viewportSizeX / viewportSizeY) : 1.0f;
        m_Projection = glm::perspective(glm::radians(m_Fov), aspect, 0.1f, 100.0f);
    }

    glm::mat4 Camera::getProjectionMatrix() {
        return m_Projection;
    }

    glm::mat4 Camera::calculateCameraOrbit()
    {
        m_CameraPos = glm::vec3(2, 5, 0);
        return glm::lookAt(m_CameraPos, glm::vec3(0.0f, 0.0f, 0.0f), m_Up);
    }
}