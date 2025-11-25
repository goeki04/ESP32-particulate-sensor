#pragma once
class GuiManager{
    public:
    void init(SDL_Window* window, SDL_Renderer* renderer);
    void draw();
    void destroy();
private:
    void drawNavBar();
    void drawNotifications();
    void drawInformation();
    int m_WindowWidth;
    int m_WindowHeight;
    float m_MenuBarHeight;
    ImGuiWindowFlags m_WindowFlags = 0;
};