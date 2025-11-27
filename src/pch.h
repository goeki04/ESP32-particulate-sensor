#pragma once
#define SDL_MAIN_USE_CALLBACKS 1
#define IMGUI_IMPL_OPENGL_LOADER_CUSTOM
#include <GL/glew.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_opengl.h>
#include <iostream>
#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_opengl3.h"
#include <algorithm>
#include <glm/glm.hpp>          
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <chrono>
#ifndef DEBUG_RENDERING_OPENGL
#define DEBUG_RENDERING_OPENGL
#endif
