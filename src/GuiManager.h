#pragma once
class GuiManager{
    public:
    bool showDemoWindow = true;
    bool showAnotherWindow = false;
    ImVec4 clearColor = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    void init(SDL_Window* window, SDL_Renderer* renderer);
    void draw();
    void destroy();
};