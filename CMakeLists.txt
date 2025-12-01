cmake_minimum_required(VERSION 3.23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(ESP32 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(SHADERS src/shader/vertexShader.glsl src/shader/fragmentShader.glsl)

find_package(glm REQUIRED)
find_package(SDL3 REQUIRED)
find_package(stb REQUIRED)
find_package(imgui REQUIRED)
find_package(glew REQUIRED)
find_package(assimp REQUIRED)

file(GLOB SOURCES "src/*.cpp")
file(GLOB HEADERS "src/*.h")
file(GLOB BINDING_SOURCES "bindings/*.cpp")
file(GLOB BINDING_HEADERS "bindings/*.h")

add_executable(ESP32 ${SOURCES} ${HEADERS} ${BINDING_SOURCES} ${BINDING_HEADERS} ${SHADERS})
source_group("Shader files" FILES ${SHADERS})
target_include_directories(ESP32 PRIVATE bindings)
target_precompile_headers(ESP32 PRIVATE src/pch.h)

target_link_libraries(ESP32 PRIVATE
    glm::glm
    sdl::sdl
    imgui::imgui
    GLEW::GLEW
    stb::stb
    assimp::assimp
)
