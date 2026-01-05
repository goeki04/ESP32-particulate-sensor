#include "WindowManager.h"
#include "pch.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
using namespace Window;
void WindowManager::start() {
    int16_t windowFlags = 0;
    SDL_Surface* surface = CreateSDLSurface("../assets/icons/logo.png");
    
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
SDL_Surface* WindowManager::CreateSDLSurface(const char* path)
{
    int w, h, channels;
    unsigned char* pixels = stbi_load(path, &w, &h, &channels, 4);
    if (!pixels) {
        SDL_Log("Failed to load PNG: %s", stbi_failure_reason());
        return NULL;
    }
    SDL_Surface* surface = SDL_CreateSurfaceFrom(w, h,SDL_PIXELFORMAT_RGBA32,pixels,w*4);
    if (!surface) {
        stbi_image_free(pixels);
        SDL_Log("Failed to create surface: %s", SDL_GetError());
        return NULL;
    }
    return surface;
}