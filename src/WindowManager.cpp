#include "WindowManager.h"
#include "pch.h"

SDL_Window* WindowManager::m_Window = NULL;
SDL_Renderer* WindowManager::m_Renderer = NULL;
void WindowManager::start() {
    if (!SDL_CreateWindowAndRenderer("ESP32", 640, 480, SDL_WINDOW_RESIZABLE, &m_Window, &m_Renderer)) {
        SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
        
    }
    SDL_SetRenderLogicalPresentation(m_Renderer, 640, 480, SDL_LOGICAL_PRESENTATION_LETTERBOX);
}

void WindowManager::update() {
    SDL_SetRenderDrawColorFloat(m_Renderer, 1.0, 1.0, 1.0, SDL_ALPHA_OPAQUE_FLOAT);
    SDL_RenderClear(m_Renderer);
    SDL_RenderPresent(m_Renderer);
}

void WindowManager::destroy() {

}