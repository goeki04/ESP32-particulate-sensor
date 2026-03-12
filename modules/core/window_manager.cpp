#include "window_manager.h"
#include "stb_image.h"
#include <cstring>
#include <stdexcept>
#include <GL/glew.h>
namespace Andromeda::Window {
    SDL_Surface* WindowManager::CreateSDLSurface(const char* path)
    {
        int w, h, channels;
        unsigned char* pixels = stbi_load(path, &w, &h, &channels, 4);
        if (!pixels) {
            SDL_Log("Failed to load image: %s", stbi_failure_reason());
            return nullptr;
        }

        SDL_Surface* surface = SDL_CreateSurface(w, h, SDL_PIXELFORMAT_RGBA32);
        if (!surface) {
            stbi_image_free(pixels);
            SDL_Log("Failed to create surface: %s", SDL_GetError());
            return nullptr;
        }
        SDL_LockSurface(surface);
        std::memcpy(surface->pixels, pixels, (size_t)w * (size_t)h * 4);
        SDL_UnlockSurface(surface);

        stbi_image_free(pixels);
        return surface;
    }
    void WindowManager::start() {
        int16_t windowFlags = 0;
        std::printf(ASSET_PATH "\n");
        SDL_Surface* surface = CreateSDLSurface(ASSET_PATH "logo.png");

        windowFlags |= SDL_WINDOW_MAXIMIZED;
        windowFlags |= SDL_WINDOW_RESIZABLE;
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        g_Window = SDL_CreateWindow("ESP32", NULL, NULL, windowFlags | SDL_WINDOW_OPENGL);
        SDL_GetWindowSizeInPixels(g_Window, &g_WindowWidth, &g_WindowHeight);
        if (!g_Window) {
            throw std::runtime_error("Failed to call SDL_CreateWindow!");
        }
        SDL_SetWindowMinimumSize(g_Window, g_WindowWidth, g_WindowHeight);
        SDL_SetWindowIcon(g_Window, surface);
        m_GlContext = SDL_GL_CreateContext(g_Window);
        if (!m_GlContext) {
            throw std::runtime_error("Failed to create SDL_GL context!");
        }
        GLenum err = glewInit();
        if (err != GLEW_OK) {
            throw std::runtime_error("Failed to call glewInit!");
        }
    }

    void WindowManager::destroy() {
        SDL_DestroyWindow(g_Window);
        SDL_Quit();
    }

    void WindowManager::updateEvent(SDL_Event* event)
    {
        //ImGui_ImplSDL3_ProcessEvent(event);
    }
}