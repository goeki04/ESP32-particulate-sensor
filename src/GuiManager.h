#pragma once
#include "glm/glm.hpp"
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

    void init(SDL_Window* window);
    void update();
    float getMenuBarHeight();
    ImVec2 getNewWindowPos(Margin margin, ImVec2 windowSize, Alignment alignment);
    void drawViewportGUI(unsigned int framebufferTexture, ImVec2 framebufferSize);
    void loadFont();
    ImVec2 getViewportWindowSize();
private:
    std::string m_ImguiVersion = "ImGui: " + std::string(IMGUI_VERSION);
    static bool m_ShowVersion;
    int m_WindowWidth = 0;
    int m_WindowHeight = 0;
    ImVec2 m_ViewportPos;
    float m_MenuBarHeight = 0;
    float m_WidgetWidth = 0;
    float m_MarginDefault = 0;
    ImGuiWindowFlags m_WindowFlags = 0;
    bool m_Debug = false;
    ImVec2 getViewportWindowPos();
    void drawNavBar();
    void OpenFolder();
    void drawNotification();
    void drawInformation();
    void drawBottomWindow();
    void drawDeviceBrowser();
    void drawDetailsPanel();
    void drawChart();
    void OpenURL(const std::string& url);
    void setViewportSize();
    void setFlags();
    void setStyle();
};

inline bool operator==(const ImVec2& a, const ImVec2& b) {
    return a.x == b.x && a.y == b.y;
}
/// <summary>
/// Used for debugging with glm
/// </summary>
/// <returns>os</returns>
inline std::ostream& operator<<(std::ostream& os,const glm::vec4& a) {
    os << "(" << a.x << ", " << a.y << ", " << a.z << ", " << a.w << ")";
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const ImVec2& a) {
    os << "(" << a.x << ", " << a.y << ")";
    return os;
}