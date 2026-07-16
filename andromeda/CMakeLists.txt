cmake_minimum_required(VERSION 3.23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(Andromeda LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(WIN32)
    add_compile_definitions(ANDROMEDA_OS="Windows")
elseif(UNIX)
    add_compile_definitions(ANDROMEDA_OS="Linux")
endif()

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    add_compile_definitions(ANDROMEDA_ARCH="x64")
else()
    add_compile_definitions(ANDROMEDA_ARCH="x86")
endif()
# Graphics backend implementing the renderer_rhi module (modules/renderer_rhi/CMakeLists.txt
# picks the matching *.cpp sources and dependencies for this). Only OpenGL has an actual
# implementation right now; Vulkan/DirectX are reserved names for future backends.
# Set via: cmake -DANDROMEDA_GRAPHICS_API=OpenGL ..
set(ANDROMEDA_GRAPHICS_API "OpenGL" CACHE STRING "Graphics backend used by modules/renderer_rhi (OpenGL, Vulkan, DirectX)")
set_property(CACHE ANDROMEDA_GRAPHICS_API PROPERTY STRINGS OpenGL Vulkan DirectX)

# NOTE: the `renderer` and `gui` modules still talk to OpenGL directly in places (raw gl*
# calls, OpenGLContext hardcoded in Renderer::initRenderer). This switch only governs which
# backend modules/renderer_rhi is built with; it does not yet make the rest of the engine
# backend-agnostic.
find_package(SDL3 REQUIRED)
find_package(protobuf REQUIRED)
find_package(glew REQUIRED)
find_package(glslang REQUIRED)
find_package(GTest REQUIRED)
find_package(httplib REQUIRED)
find_package(spdlog REQUIRED)
find_package(spirv-reflect-static REQUIRED)
file(GLOB SOURCES "modules/*.cpp")
file(GLOB HEADERS "modules/*.h")


get_filename_component(PROTO_DIR "${CMAKE_CURRENT_SOURCE_DIR}/proto" ABSOLUTE)
set(PROTO_FILES 
    proto/api_options.proto 
    proto/api.proto
)
set(PROTOBUF_GENERATE_CPP_APPEND_PATH TRUE)
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${PROTO_FILES})

add_subdirectory(modules/network)
add_subdirectory(modules/core)
add_subdirectory(modules/definitions)
add_subdirectory(modules/renderer)
add_subdirectory(modules/scene)
add_subdirectory(modules/gui)
add_subdirectory(modules/editor)
add_subdirectory(modules/input)
add_subdirectory(modules/resource)
add_subdirectory(modules/filesystem)
add_subdirectory(modules/serialization)
add_subdirectory(modules/renderer_rhi)

add_executable(AndromedaReflector tools/shader_reflector/main.cpp)
target_link_libraries(AndromedaReflector PRIVATE spirv-reflect-static)
target_link_libraries(AndromedaReflector PRIVATE definitions)
target_compile_definitions(AndromedaReflector PRIVATE SHADER_PATH="${CMAKE_SOURCE_DIR}/modules/shader/")


add_executable(Andromeda 
    ${SOURCES} 
    ${HEADERS}
    ${BINDING_SOURCES} 
    ${BINDING_HEADERS} 
    ${SHADERS}
    ${PROTO_SRCS} 
    ${PROTO_HDRS}
        modules/definitions/Math/a_glm_json_parser.hpp
        modules/definitions/Math/a_glm_json_parser.hpp
)
add_dependencies(Andromeda GenerateShaderHeaders)
target_include_directories(Andromeda PUBLIC ${CMAKE_BINARY_DIR}/generated)

file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/generated)
file(GLOB_RECURSE ALL_GLSL_SHADERS
    CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/modules/shader/*.vert"
    "${CMAKE_SOURCE_DIR}/modules/shader/*.frag"
    "${CMAKE_SOURCE_DIR}/modules/shader/*.comp"
    "${CMAKE_SOURCE_DIR}/modules/shader/*.hlsl"
)
set(ALL_SPV_FILES "")
foreach(SHADER ${ALL_GLSL_SHADERS})
    get_filename_component(SHADER_NAME ${SHADER} NAME)
    list(APPEND ALL_SPV_FILES "${CMAKE_BINARY_DIR}/shaders/spirv/${SHADER_NAME}.spv")
endforeach()
set(GENERATED_HEADER ${CMAKE_BINARY_DIR}/generated/a_shader_generated.hpp)
set(GENERATED_COMPUTE_HEADER ${CMAKE_BINARY_DIR}/generated/a_shader_generated_compute.hpp)
message(STATUS "===== SHADER REFLECTOR DEBUG =====")
message(STATUS "CMAKE_BINARY_DIR: ${CMAKE_BINARY_DIR}")
message(STATUS "Looking for SPV files in: ${CMAKE_BINARY_DIR}/shaders/spirv/*.spv")
message(STATUS "ALL_SPV_FILES: ${ALL_SPV_FILES}")
message(STATUS "Number of SPV files found: ${CMAKE_ARGC}")
if(ALL_SPV_FILES)
    add_custom_command(
        OUTPUT ${GENERATED_HEADER} ${GENERATED_COMPUTE_HEADER}
        COMMAND AndromedaReflector ${GENERATED_HEADER} ${GENERATED_COMPUTE_HEADER} ${ALL_SPV_FILES}
        DEPENDS AndromedaReflector ${ALL_SPV_FILES}
        COMMENT "Andromeda Engine: Generating Reflected Shader Structs for all shaders..."
    )
else()
    add_custom_command(
        OUTPUT ${GENERATED_HEADER} ${GENERATED_COMPUTE_HEADER}
        COMMAND ${CMAKE_COMMAND} -E echo "#pragma once" > ${GENERATED_HEADER}
        COMMAND ${CMAKE_COMMAND} -E echo "namespace Andromeda::Generated {}" >> ${GENERATED_HEADER}
        COMMAND ${CMAKE_COMMAND} -E echo "#pragma once" > ${GENERATED_COMPUTE_HEADER}
        COMMAND ${CMAKE_COMMAND} -E echo "namespace Andromeda::Generated::Compute {}" >> ${GENERATED_COMPUTE_HEADER}
        COMMENT "Andromeda Engine: No SPIR-V files found, creating empty fallback headers."
    )
endif()

add_custom_target(GenerateShaderHeaders ALL DEPENDS ${GENERATED_HEADER} ${GENERATED_COMPUTE_HEADER})
add_dependencies(GenerateShaderHeaders renderer_rhi_shaders)

# These static libs include the generated header (a_shader_generated.hpp), so
# they must wait for GenerateShaderHeaders. Without this, a clean parallel build
# can compile them before the header exists (fatal C1083) - see CI failure.
add_dependencies(renderer GenerateShaderHeaders)
add_dependencies(gui GenerateShaderHeaders)
target_include_directories(Andromeda PRIVATE ${CMAKE_CURRENT_BINARY_DIR} ${PROTOBUF_INCLUDE_DIRS})
target_include_directories(Andromeda PRIVATE 
    ${CMAKE_CURRENT_BINARY_DIR}/proto 
)
target_link_libraries(Andromeda PRIVATE
    #External dependencies
    sdl::sdl
    GLEW::GLEW
    protobuf::libprotobuf
    glslang::_glslang-do-not-use
    spdlog::spdlog
    #Engine modules
    network
    core
    renderer
    editor
    input
    renderer_rhi
    serialization
    filesystem
    resource
    definitions
)

enable_testing()
include(GoogleTest)

set(ANDROMEDA_TEST_SOURCES
    "tests/test_ecs.cpp"
    "tests/test_math.cpp"
    "tests/test_resource.cpp"
    "tests/test_rhi.cpp"
    "tests/test_boost_websocket_client.cpp"
)

add_executable(AndromedaTests ${ANDROMEDA_TEST_SOURCES})
target_compile_definitions(resource PRIVATE SHADER_PATH="${CMAKE_CURRENT_SOURCE_DIR}/modules/shader/")

# Headers under test are header-only (ECS, math, geometry, primitive generator,
# RHI types), so we only need their include directories plus glm/nlohmann (via
# the definitions interface target). No GL context is required at runtime.
target_include_directories(AndromedaTests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/modules/core
    ${CMAKE_CURRENT_SOURCE_DIR}/modules/resource
    ${CMAKE_CURRENT_SOURCE_DIR}/modules/renderer_rhi
)
target_link_libraries(AndromedaTests PRIVATE
    GTest::gtest_main
    definitions
    network
)

gtest_discover_tests(AndromedaTests)
