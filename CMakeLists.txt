cmake_minimum_required(VERSION 3.23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(Andromeda LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(glm REQUIRED)
find_package(SDL3 REQUIRED)
find_package(stb REQUIRED)
find_package(imgui REQUIRED)
find_package(protobuf REQUIRED)
find_package(glew REQUIRED)
find_package(assimp REQUIRED)

file(GLOB SHADERS "modules/shader/*.glsl")
file(GLOB SHADERS "modules/shader/compute/*.compute")
file(GLOB SOURCES "modules/*.cpp")
file(GLOB HEADERS "modules/*.h")
file(GLOB BINDING_SOURCES "bindings/*.cpp")
file(GLOB BINDING_HEADERS "bindings/*.h")

get_filename_component(PROTO_DIR "${CMAKE_CURRENT_SOURCE_DIR}/proto" ABSOLUTE)
set(PROTO_FILES 
    proto/api_options.proto 
    proto/api.proto
)
set(PROTOBUF_GENERATE_CPP_APPEND_PATH TRUE)
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${PROTO_FILES})

add_subdirectory(modules/network)
add_subdirectory(modules/core)
add_subdirectory(modules/collision)
add_subdirectory(modules/definitions)
add_subdirectory(modules/renderer)
add_subdirectory(modules/scene)
add_subdirectory(modules/editor)
add_subdirectory(modules/resource)
add_subdirectory(modules/filesystem)
add_subdirectory(modules/renderer_rhi)
add_executable(Andromeda 
    ${SOURCES} 
    ${HEADERS} 
    ${BINDING_SOURCES} 
    ${BINDING_HEADERS} 
    ${SHADERS}
    ${PROTO_SRCS} 
    ${PROTO_HDRS}
)

target_compile_definitions(Andromeda PRIVATE ASSET_PATH="${CMAKE_CURRENT_SOURCE_DIR}/assets/")
target_compile_definitions(Andromeda PRIVATE SHADER_PATH="${CMAKE_CURRENT_SOURCE_DIR}/modules/shader/")
source_group("Shader files" FILES ${SHADERS})
target_include_directories(Andromeda PRIVATE bindings)
target_include_directories(Andromeda PRIVATE ${CMAKE_CURRENT_BINARY_DIR} ${PROTOBUF_INCLUDE_DIRS})
target_include_directories(Andromeda PRIVATE 
    ${CMAKE_CURRENT_BINARY_DIR}/proto 
)
target_precompile_headers(Andromeda PRIVATE modules/pch.h)
target_link_libraries(Andromeda PRIVATE
    #External dependencies
    glm::glm
    sdl::sdl
    imgui::imgui
    GLEW::GLEW
    protobuf::libprotobuf
    stb::stb
    assimp::assimp
    
    #Engine modules
    network
    core
    renderer
    renderer_rhi
    collision
    filesystem
    resource
    definitions
)
