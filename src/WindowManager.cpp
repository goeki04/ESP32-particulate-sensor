#include "WindowManager.h"
#include "pch.h"
void WindowManager::start() {
    int16_t windowFlags = 0;
    windowFlags |= SDL_WINDOW_MAXIMIZED;
    windowFlags |= SDL_WINDOW_RESIZABLE;
    if (!SDL_CreateWindowAndRenderer("ESP32", NULL, NULL, windowFlags, &m_Window, &m_SDLRenderer)) {
        SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
    }
    m_GuiManager.init(m_Window,m_SDLRenderer);
    SDL_SetWindowMinimumSize(m_Window,800,600);
}

void WindowManager::update() {
    ImGuiIO& io = ImGui::GetIO();(void)io;
    m_GuiManager.draw();
    ImGui::Render();
    SDL_SetRenderScale(m_SDLRenderer, io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
    SDL_SetRenderDrawColorFloat(m_SDLRenderer, 0.5f, 0.5f, 0.5f, 1.0f);
    SDL_RenderClear(m_SDLRenderer);
    ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), m_SDLRenderer);
    SDL_RenderPresent(m_SDLRenderer);
}

void WindowManager::destroy() {
    m_GuiManager.destroy();
    SDL_DestroyRenderer(m_SDLRenderer);
    SDL_DestroyWindow(m_Window);
    SDL_Quit();
}

void WindowManager::updateEvent(SDL_Event* event)
{
    ImGui_ImplSDL3_ProcessEvent(event);
}
