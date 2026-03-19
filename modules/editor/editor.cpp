#include "a_graphics_base.hpp"
#include "editor.hpp"
#include "renderer.h"
#include "window_manager.h"
#include "Scene.hpp"
#include "resource_manager.h"
#include "subsystem_manager.h"
#include "a_guiTypes.hpp"
#include "registry.h"
namespace Andromeda {
	void Editor::start()
	{
        assert(Window::m_GlContext && "OpenGL context is not initialized!");
		m_Renderer = SystemManager::getInstance().getSubsystem<Renderer>();
        assert(m_Renderer && "m_Renderer is nullptr in Editor::Start()");
        m_SceneManager = SystemManager::getInstance().getSubsystem<SceneManager>();
        assert(m_SceneManager && "m_SceneManager is nullptr in Editor::Start()");
        ResourceManager* rm = SystemManager::getInstance().getSubsystem<ResourceManager>();
        assert(rm && "rm is nullptr in Editor::Start()");

        Gui::GuiRendererConfig guiConfig;
        guiConfig.cam = &m_SceneManager->m_EditorCamData;
        guiConfig.glsl_version = m_Renderer->glsl_version;
        guiConfig.sdl_gl_context = Window::m_GlContext;
        guiConfig.registry = &m_SceneManager->m_Registry;
        guiConfig.window = Window::g_Window;
        guiConfig.dp = rm;
		m_GuiRenderer.init(guiConfig);
	}
	void Editor::update()
	{
        if (Gui::GuiRenderer::s_ViewportFocused) {
            cameraMovement(m_SceneManager->m_EditorCamData);
        }

		vec2 viewportSize = m_GuiRenderer.getViewportWindowSize();
        setProjectionMatrix(m_SceneManager->m_EditorCamData, viewportSize.x, viewportSize.y);
        updatePickingRay(m_SceneManager->m_EditorCamData);
        editorPicking(&m_SceneManager->m_EditorCamData);
        Gui::ViewportDrawInfo vpInfo;
        vpInfo.camData = &m_SceneManager->m_EditorCamData;
        vpInfo.framebufferSize = m_Renderer->m_FramebufferSize;
        vpInfo.postProcessingFboTexture = m_Renderer->fboManager.m_PostprocessTexture;
        m_GuiRenderer.update(vpInfo);
	}
    
    void Editor::updateEvent(SDL_Event* event) {
        ImGui_ImplSDL3_ProcessEvent(event);
    }

	void Editor::destroy()
	{
		m_GuiRenderer.destroy();
	}

    inline bool Editor::RayIntersectAABB(const amath::CameraData& cam, const ECS::Component::AABB& aabb, const glm::mat4& modelMatrix)
    {
        const amath::Ray& rayW = cam.m_CursorToWorldRay;

        mat4 invModel = glm::inverse(modelMatrix);

        vec3 o = glm::vec3(invModel * vec4(rayW.origin, 1.0f));
        vec3 d = glm::normalize(vec3(invModel * vec4(rayW.direction, 0.0f)));

        float tMin = 0.001f;
        float tMax = 1e30f;

        for (int i = 0; i < 3; ++i)
        {
            float oi = o[i];
            float di = d[i];

            if (std::fabs(di) < 1e-8f) {
                if (oi < aabb.min[i] || oi > aabb.max[i]) return false;
                continue;
            }

            float invD = 1.0f / di;
            float t0 = (aabb.min[i] - oi) * invD;
            float t1 = (aabb.max[i] - oi) * invD;
            if (t0 > t1) std::swap(t0, t1);

            tMin = std::max(tMin, t0);
            tMax = std::min(tMax, t1);
            if (tMin > tMax) return false;
        }
        return true;
    }
    void Editor::editorPicking(const amath::CameraData* cam)
    {
        if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
            auto& aabbPool = m_SceneManager->m_Registry.getPool<ECS::Component::AABB>();
            auto& selectedPool = m_SceneManager->m_Registry.getPool<ECS::Component::Selected>();
            std::vector<Entity> currentlySelected = selectedPool.getEntities();
            for (Entity e : currentlySelected) {
                m_SceneManager->m_Registry.getPool<ECS::Component::Selected>().removeEntity(e);
            }
            bool anyHit = false;


            for (Entity e : aabbPool.getEntities()) {
                ECS::EntityHandle handle = { e,&m_SceneManager->m_Registry };
                if (!handle.has<ECS::Component::Transform>()) {
                    continue;
                }
                const mat4 modelMatrix = handle.get<ECS::Component::Transform>().modelMatrix();
                const auto& aabb = handle.get<ECS::Component::AABB>();

                if (RayIntersectAABB(*cam, aabb, modelMatrix)) {
                    handle.add<ECS::Component::Selected>({});
                    m_GuiRenderer.m_CurrentSelectedID = e;
                    anyHit = true;
                    break;
                }
            }

            if (!anyHit) {
                m_GuiRenderer.m_CurrentSelectedID = -1;
            }
        }
    }
    amath::Ray Editor::cursorToWorldRay(const amath::CameraData& cam) const
    {
        float x = (2.0f * cam.m_ImGuiMouseX) / cam.m_framebufferSize.x - 1.0f;
        float y = 1.0f - (2.0f * cam.m_ImGuiMouseY) / cam.m_framebufferSize.y;
        vec4 rayClip(x, y, -1.0f, 1.0f);

        vec4 rayView = amath::inverse(cam.m_Projection) * rayClip;
        rayView = vec4(rayView.x, rayView.y, -1.0f, 0.0f);
        vec4 rayDir4 = amath::inverse(cam.m_ViewMatrix) * rayView;
        amath::Ray ray{ vec3(0.0f),vec3(0.0f) };

        ray.direction = amath::normalize(vec3(rayDir4));
        ray.origin = cam.m_CameraPos;
        return ray;
    }
    //call this function after m_EditorCamData has been initialized
    void Editor::updateEditorCameraRay()
    {
        if (m_SceneManager->m_EditorCamData.m_HasValidPickRay) {
            m_SceneManager->m_EditorCamData.m_CursorToWorldRay = cursorToWorldRay(m_SceneManager->m_EditorCamData);
        }
    }
    bool Editor::RayIntersectsXZPlane(const amath::Ray& ray, float planeY, vec3& outHitPoint)
    {
        if (amath::abs(ray.direction.y) < 1e-6f) {
            return false;
        }
        float t = (planeY - ray.origin.y) / ray.direction.y;
        if (t < 0.0f) {
            return false;
        }

        outHitPoint = ray.origin + t * ray.direction;
        return true;
    }
    void Editor::cameraMovement(amath::CameraData& cam)
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
            SDL_GetMouseState(&cam.m_LastMouseX, &cam.m_LastMouseY);
            SDL_SetWindowRelativeMouseMode(SDL_GL_GetCurrentWindow(), true);

            SDL_GetRelativeMouseState(NULL, NULL);
        }
        else if (!rightMouseDown && wasRightMouseDown) {
            ImGuiIO& io = ImGui::GetIO(); (void)io;
            io.ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
            SDL_WarpMouseInWindow(currentWindow, cam.m_LastMouseX, cam.m_LastMouseY);
            SDL_SetWindowRelativeMouseMode(SDL_GL_GetCurrentWindow(), false);
        }
        wasRightMouseDown = rightMouseDown;

        if (rightMouseDown) {
            float relX, relY;
            SDL_GetRelativeMouseState(&relX, &relY);

            cam.m_Yaw += relX * cam.m_Sensitivity;
            cam.m_Pitch -= relY * cam.m_Sensitivity;
            cam.m_Pitch = std::clamp(cam.m_Pitch, -89.0f, 89.0f);
        }

        float radYaw = glm::radians(cam.m_Yaw);
        float radPitch = glm::radians(cam.m_Pitch);

        cam.m_Forward.x = cos(radYaw) * cos(radPitch);
        cam.m_Forward.y = sin(radPitch);
        cam.m_Forward.z = sin(radYaw) * cos(radPitch);
        cam.m_Forward = amath::normalize(cam.m_Forward);

        cam.m_Right = amath::normalize(amath::cross(cam.m_Forward, glm::vec3(0.0f, 1.0f, 0.0f)));

        const bool* keyboard = SDL_GetKeyboardState(NULL);
        glm::vec3 moveDir(0.0f);

        if (keyboard[SDL_SCANCODE_W]) moveDir += cam.m_Forward;
        if (keyboard[SDL_SCANCODE_S]) moveDir -= cam.m_Forward;
        if (keyboard[SDL_SCANCODE_A]) moveDir -= cam.m_Right;
        if (keyboard[SDL_SCANCODE_D]) moveDir += cam.m_Right;

        if (glm::length(moveDir) > 0.0f) {
            moveDir = glm::normalize(moveDir);
            cam.m_CameraPos += moveDir * cam.m_Speed * SystemManager::s_deltaTime;
        }

        cam.m_Target = cam.m_CameraPos + cam.m_Forward;
        cam.m_ViewMatrix = amath::lookAt(cam.m_CameraPos, cam.m_Target, vec3(0.0f, 1.0f, 0.0f));
    }

    vec3 Editor::getCameraPos(const amath::CameraData& cam) const
    {
        return cam.m_CameraPos;
    }

    mat4 Editor::getViewMatrix(const amath::CameraData& cam) const
    {
        return cam.m_ViewMatrix;
    }

    void Editor::updatePickingRay(amath::CameraData& cam)
    {
        ImVec2 mouse = ImGui::GetMousePos();

        float localX = mouse.x - cam.m_ViewportPos.x;
        float localY = mouse.y - cam.m_ViewportPos.y;

        if (localX < 0 || localY < 0 || localX >= cam.m_ViewportSize.x || localY >= cam.m_ViewportSize.y) {
            cam.m_HasValidPickRay = false;
            return;
        }

        cam.m_HasValidPickRay = true;
        cam.m_ImGuiMouseX = localX;
        cam.m_ImGuiMouseY = localY;
        cam.m_framebufferSize = vec2(cam.m_ViewportSize.x, cam.m_ViewportSize.y);
        cam.m_CursorToWorldRay = cursorToWorldRay(cam);
    }

    void Editor::zoom(amath::CameraData& cam,SDL_Event* event) {
        if (event->type == SDL_EVENT_MOUSE_WHEEL) {
            if (event->wheel.y > 0) {
                cam.m_Fov = std::clamp(cam.m_Fov + 1, cam.m_FovMin, cam.m_FovMax);
            }
            else if (event->wheel.y < 0) {
                cam.m_Fov = std::clamp(cam.m_Fov - 1, cam.m_FovMin, cam.m_FovMax);
            }
        }
        setProjectionMatrix(cam,cam.m_ViewportSize.x, cam.m_ViewportSize.y);
    }

    void Editor::setProjectionMatrix(amath::CameraData& cam,float viewportSizeX, float viewportSizeY) const {
        float aspect = (viewportSizeY > 0) ? (viewportSizeX / viewportSizeY) : 1.0f;
        cam.m_Projection = amath::perspective(amath::radians(cam.m_Fov), aspect, 0.1f, 100.0f);
    }

    mat4 Editor::getProjectionMatrix(const amath::CameraData& cam) {
        return cam.m_Projection;
    }

    mat4 Editor::calculateCameraOrbit(amath::CameraData& cam)
    {
        cam.m_CameraPos = vec3(2, 5, 0);
        return amath::lookAt(cam.m_CameraPos, vec3(0.0f, 0.0f, 0.0f), cam.m_Up);
    }
}