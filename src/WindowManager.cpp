#include "WindowManager.h"
#include "pch.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
void WindowManager::start() {
    int16_t windowFlags = 0;
    SDL_Surface* surface = LoadPNG("../assets/logo.png");
    
    windowFlags |= SDL_WINDOW_MAXIMIZED;
    windowFlags |= SDL_WINDOW_RESIZABLE;
    if (!SDL_CreateWindowAndRenderer("ESP32", NULL, NULL, windowFlags, &m_Window, &m_SDLRenderer)) {
        SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
    }
    SDL_SetWindowIcon(m_Window, surface);
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

SDL_Surface* WindowManager::LoadPNG(const char* path)
{
    int w, h, channels;
    unsigned char* pixels = stbi_load(path, &w, &h, &channels, 4); // RGBA
    if (!pixels) {
        SDL_Log("Failed to load PNG: %s", stbi_failure_reason());
        return NULL;
    }

    // Create SDL surface from pixel data
    SDL_Surface* surface = SDL_CreateSurfaceFrom(w, h,SDL_PIXELFORMAT_RGBA32,pixels,w*4);

    if (!surface) {
        stbi_image_free(pixels);
        SDL_Log("Failed to create surface: %s", SDL_GetError());
        return NULL;
    }

    return surface;
}