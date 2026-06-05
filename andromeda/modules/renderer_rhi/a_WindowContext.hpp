#pragma once
#include <SDL3/SDL_video.h>

/**
 * @brief Bundles graphics API and OS window handles.
 */
struct WindowContext
{
    /** @brief Pointer to the SDL window instance. */
    SDL_Window* window = nullptr;

    /** @brief The OpenGL context associated with the window. SDL_GLContext ist declared as pointer in SDL3*/
    SDL_GLContext glContext = nullptr;

    /** @brief String representing the GLSL version (e.g., "#version 410"). */
    const char* glslVersion = nullptr;
};
