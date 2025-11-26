#pragma once
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
    void init(SDL_Window* window);
    void update();
private:
    void drawNavBar();
    void drawNotification();
    void drawInformation();
    void drawChart();
    void drawMeasurementDisplay();
    void setFlags();
    void setStyle();
    ImVec2 getNewWindowPos(Margin margin, ImVec2 windowPos, ImVec2 windowSize, Alignment alignment);
    int m_WindowWidth = 0;
    int m_WindowHeight = 0;
    float m_MenuBarHeight = 0;
    float m_WidgetWidth = 0;
    float m_MarginDefault = 0;
    ImGuiWindowFlags m_WindowFlags = 0;
};