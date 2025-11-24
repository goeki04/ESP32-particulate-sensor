#include "WindowManager.h"
#include "pch.h"
void WindowManager::start() {
    if (!SDL_CreateWindowAndRenderer("ESP32", 640, 480, SDL_WINDOW_RESIZABLE, &m_Window, &m_Renderer.m_SDLRenderer)) {
        SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
    }
    std::cout <<m_Renderer.m_SDLRenderer;

    SDL_SetRenderLogicalPresentation(m_Renderer.m_SDLRenderer, 640, 480, SDL_LOGICAL_PRESENTATION_LETTERBOX);
}

void WindowManager::update() {
    m_Renderer.draw();
}

void WindowManager::destroy() {
    SDL_DestroyRenderer(m_Renderer.m_SDLRenderer);
    SDL_DestroyWindow(m_Window);
    SDL_Quit();
}

SDL_Renderer* WindowManager::getRenderer()
{
    return m_Renderer.m_SDLRenderer;
}

SDL_Window* WindowManager::getWindow()
{
    return m_Window;
}
