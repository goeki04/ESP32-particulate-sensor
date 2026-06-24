#pragma once

/**
 * @file a_graphics_base.hpp
 * @brief Common include hub for the graphics stack: GLEW, SDL3, OpenGL and the ImGui SDL/OpenGL backends.
 *
 * @details Pulled in by code that needs the raw graphics/UI headers together in the correct order.
 *          Defining @c IMGUI_IMPL_OPENGL_LOADER_CUSTOM tells ImGui's OpenGL3 backend that the loader
 *          (GLEW) is provided by the application rather than ImGui's built-in loader.
 */

#ifndef IMGUI_IMPL_OPENGL_LOADER_CUSTOM
	#define IMGUI_IMPL_OPENGL_LOADER_CUSTOM
#endif

#include <GL/glew.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>

#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_opengl3.h"