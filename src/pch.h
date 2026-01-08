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
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <fstream>
#include <filesystem>
#include <span>
#ifndef DEBUG_RENDERING_OPENGL
#define DEBUG_RENDERING_OPENGL
#endif

#if defined(_WIN32)
constexpr const char* g_os = "Windows";
#elif defined(__linux__)
constexpr const char* g_os = "Linux";
#elif defined(__APPLE__)
constexpr const char* g_os = "macOS";
#endif

#if defined(_M_X64) || defined(__x86_64__)
constexpr const char* g_arch = "x64";
#elif defined(_M_IX86) || defined(__i386__)
constexpr const char* g_arch = "x86";
#elif defined(__aarch64__) || defined(_M_ARM64)
constexpr const char* g_arch = "ARM64";
#elif defined(__arm__)
constexpr const char* g_arch = "ARM32";
#else
constexpr const char* g_arch = "Unknown";
#endif
