cmake_minimum_required(VERSION 3.23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(ESP32 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


find_package(glm REQUIRED)
find_package(SDL3 REQUIRED)
find_package(stb REQUIRED)
find_package(imgui REQUIRED)
find_package(protobuf REQUIRED)
find_package(asio REQUIRED)
find_package(glew REQUIRED)
find_package(assimp REQUIRED)

file(GLOB SHADERS "src/shader/*.glsl")
file(GLOB SHADERS "src/shader/compute/*.compute")
file(GLOB SOURCES "src/*.cpp")
file(GLOB HEADERS "src/*.h")
file(GLOB BINDING_SOURCES "bindings/*.cpp")
file(GLOB BINDING_HEADERS "bindings/*.h")

get_filename_component(PROTO_DIR "${CMAKE_CURRENT_SOURCE_DIR}/proto" ABSOLUTE)
set(PROTO_FILES 
    proto/api_options.proto 
    proto/api.proto
)
set(PROTOBUF_GENERATE_CPP_APPEND_PATH TRUE)
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${PROTO_FILES})

add_executable(ESP32 
    ${SOURCES} 
    ${HEADERS} 
    ${BINDING_SOURCES} 
    ${BINDING_HEADERS} 
    ${SHADERS}
    ${PROTO_SRCS} 
    ${PROTO_HDRS}
)

source_group("Shader files" FILES ${SHADERS})
target_include_directories(ESP32 PRIVATE bindings)
target_include_directories(ESP32 PRIVATE ${CMAKE_CURRENT_BINARY_DIR} ${PROTOBUF_INCLUDE_DIRS})
target_include_directories(ESP32 PRIVATE 
    ${CMAKE_CURRENT_BINARY_DIR}/proto 
)
target_precompile_headers(ESP32 PRIVATE src/pch.h)

target_link_libraries(ESP32 PRIVATE
    glm::glm
    sdl::sdl
    imgui::imgui
    GLEW::GLEW
    protobuf::libprotobuf
    asio::asio
    stb::stb
    assimp::assimp
)
