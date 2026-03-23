#pragma once
#include "a_graphics_base.hpp"
#include <vector>
#include <string>
#include "registry.h"
#include "a_math.hpp"
#include "a_guiTypes.hpp"
class IDeviceProvider;
class SceneManager;
namespace Andromeda {
    class amath::CameraData;
}
inline ImVec2 operator+(const ImVec2& a, const ImVec2& b) {
    return ImVec2(a.x + b.x, a.y + b.y);
}
inline ImVec2 operator-(const ImVec2& a, const ImVec2& b) {
    return ImVec2(a.x - b.x, a.y - b.y);
}
inline bool operator==(const ImVec2& a, const ImVec2& b) {
    if (a.x == b.x && a.y == b.y) return true;
    return false;
}
inline ImVec2 operator*(const ImVec2& a, const float b) {
    return ImVec2(a.x * b, a.y * b);
}
namespace Andromeda::Gui {

    class GuiRenderer {
    public:
        static ImVec2 s_ViewportSize;
        static bool s_ViewportFocused;

        /// <summary>
        /// This is the coordinate of the viewport pivot relative to the screen (topLeft = 0)
        /// </summary>
        vec2 m_ViewportRectMin = vec2(0.0f, 0.0f);
        void init(GuiRendererConfig& guiConfig);
        void update(ViewportDrawInfo vpInfo);
        void EndFrame(SDL_Window* window);
        void destroy();
        float getMenuBarHeight();
        ImVec2 getNewWindowPos(Margin margin, ImVec2 windowSize, Alignment alignment);
        
        void loadFont();
        vec2 getViewportWindowSize();
        int m_CurrentSelectedID = 0;
        static vec3 m_LastHitPoint;
        static bool m_HasLastHitpoint;
        amath::CameraData* m_Cam = nullptr;
        ECS::ComponentRegistry* m_Registry = nullptr;
        
        std::vector<SDL_Surface> m_DeviceIcons;
        std::string m_ImguiVersion = "ImGui: " + std::string(IMGUI_VERSION);
        IDeviceProvider* m_DeviceProvider = nullptr;
        SceneManager* m_SceneManager = nullptr;
        static bool m_ShowVersion;
        int m_WindowWidth = 0;
        int m_WindowHeight = 0;
        ImVec2 m_ViewportPos;
        float m_MenuBarHeight = 0;
        int m_SelectedIdx = -1;

        float m_WidgetWidth = 0;
        float m_MarginDefault = 0;
        ImGuiWindowFlags m_WindowFlags = 0;
        bool m_Debug = false;
        bool m_ConsoleOpen = false;
        ImVec2 getViewportWindowPos();
        void OpenFolder() const;
        void drawNotification();
        void OpenURL(const std::string& url);
        void drawChart();
        void setViewportSize() const;
        void setFlags();
        void setStyle();
    };
}