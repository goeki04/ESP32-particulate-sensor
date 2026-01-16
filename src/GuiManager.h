#pragma once
#include "glm/glm.hpp"
class ResourceManager;
struct Margin {
    float left;
    float right;
    float bottom;
    float top;
    Margin() {
        left = 0;
        right = 0;
        top = 0;
        bottom = 0;
    }
    Margin(float margin) : left(margin), right(margin), top(margin), bottom(margin) {
    }
    Margin(float left, float right, float bottom, float top)
        : left(left), right(right), bottom(bottom), top(top) {
    }
};

enum Alignment {
    TopLeft,
    TopRight,
    Center,
    CenterBottom,
    CenterTop,
    BottomLeft,
    BottomRight,
};

class GuiManager{
public:
    static ImVec2 s_ViewportSize;
    static bool s_ViewportFocused;

    /// <summary>
    /// This is the coordinate of the viewport pivot relative to the screen (topLeft = 0)
    /// </summary>
    glm::vec2 m_ViewportRectMin;
    void init(SDL_Window* window, ResourceManager* rm);
    void update();
    void destroy();
    float getMenuBarHeight();
    ImVec2 getNewWindowPos(Margin margin, ImVec2 windowSize, Alignment alignment);
    void drawViewportGUI(unsigned int framebufferTexture, ImVec2 framebufferSize, float* ImGuiMouseX, float* ImGuiMouseY);
    void loadFont();
    ImVec2 getViewportWindowSize();
    int m_CurrentSelectedID = 0;
private:
    static glm::vec3 m_LastHitPoint;
    static bool m_HasLastHitpoint;
    ResourceManager* m_ResourceManager = nullptr;
    std::vector<SDL_Surface> m_DeviceIcons;
    std::string m_ImguiVersion = "ImGui: " + std::string(IMGUI_VERSION);
    static bool m_ShowVersion;
    ImFont* m_DeviceBrowserFont = nullptr;
    ImFont* m_HeaderFont = nullptr;
    int m_WindowWidth = 0;
    int m_WindowHeight = 0;
    ImVec2 m_ViewportPos;
    float m_MenuBarHeight = 0;
    int m_SelectedIdx = -1;

    float m_WidgetWidth = 0;
    float m_MarginDefault = 0;
    ImGuiWindowFlags m_WindowFlags = 0;
    bool m_Debug = false;
    ImVec2 getViewportWindowPos();
    void drawNavBar();
    void OpenFolder();
    void drawNotification();
    void drawDeviceHierarchy();
    void drawBottomWindow();
    void drawDeviceBrowser();
    void drawDetailsPanel();
    void drawChart();
    void OpenURL(const std::string& url);
    void setViewportSize() const;
    void setFlags();
    void setStyle();
};