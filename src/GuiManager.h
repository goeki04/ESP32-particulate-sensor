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
    void init(SDL_Window* window, SDL_Renderer* renderer);
    void draw();
    void destroy();
private:
    void drawNavBar();
    void drawNotification();
    void drawInformation();
    void drawChart();
    void drawMeasurementDisplay();
    void setFlags();
    void setStyle();
    ImVec2 getNewWindowPos(Margin margin, ImVec2 windowPos, ImVec2 windowSize, Alignment alignment);
    int m_WindowWidth;
    int m_WindowHeight;
    float m_MenuBarHeight;
    float m_WidgetWidth;
    float m_MarginDefault;
    ImGuiWindowFlags m_WindowFlags = 0;
};