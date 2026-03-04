#include "WindowManager.h"
#include "pch.h"
#include "ResourceManager.h"
using namespace Window;
void WindowManager::start() {
    int16_t windowFlags = 0;
    SDL_Surface* surface = ResourceManager::CreateSDLSurface(ASSET_PATH "logo.png");
    
    windowFlags |= SDL_WINDOW_MAXIMIZED;
    windowFlags |= SDL_WINDOW_RESIZABLE;
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    g_Window = SDL_CreateWindow("ESP32", NULL, NULL, windowFlags | SDL_WINDOW_OPENGL);
    SDL_GetWindowSizeInPixels(g_Window,&g_WindowWidth,&g_WindowHeight);
    if (!g_Window) {
        throw std::exception("Failed to call SDL_CreateWindow!");
    }
    SDL_SetWindowMinimumSize(g_Window, g_WindowWidth, g_WindowHeight);
    SDL_SetWindowIcon(g_Window, surface);
}

void WindowManager::destroy() {
    SDL_DestroyWindow(g_Window);
    SDL_Quit();
}

void WindowManager::updateEvent(SDL_Event* event)
{
    ImGui_ImplSDL3_ProcessEvent(event);
}