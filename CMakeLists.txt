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

find_package(SDL3 REQUIRED)
find_package(protobuf REQUIRED)
find_package(glew REQUIRED)
find_package(glslang REQUIRED)
find_package(GTest REQUIRED)
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
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/generated)
file(GLOB_RECURSE ALL_SPV_FILES "${CMAKE_BINARY_DIR}/shaders/spirv/*.spv")
set(GENERATED_HEADER ${CMAKE_BINARY_DIR}/generated/a_shader_generated.hpp)
file(WRITE ${GENERATED_HEADER} "#pragma once\n#include \"a_primitives.hpp\"\nnamespace Andromeda::Generated {\n\n")

if(ALL_SPV_FILES)
    add_custom_command(
        OUTPUT ${GENERATED_HEADER}_trigger
        COMMAND AndromedaReflector ${GENERATED_HEADER} ${ALL_SPV_FILES}
        COMMAND ${CMAKE_COMMAND} -E echo "} // namespace Andromeda::Generated" >> ${GENERATED_HEADER}
        DEPENDS AndromedaReflector ${ALL_SPV_FILES}
        COMMENT "Andromeda Engine: Generating Reflected Shader Structs for all shaders..."
    )
else()
    add_custom_command(
        OUTPUT ${GENERATED_HEADER}_trigger
        COMMAND ${CMAKE_COMMAND} -E echo "} // namespace Andromeda::Generated" >> ${GENERATED_HEADER}
        COMMENT "Andromeda Engine: No SPIR-V files found, creating empty header."
    )
endif()

add_custom_target(GenerateShaderHeaders ALL DEPENDS ${GENERATED_HEADER}_trigger)
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
    "tests/test_system_start.cpp"
) 

add_executable(AndromedaTests ${ANDROMEDA_TEST_SOURCES})
target_compile_definitions(AndromedaTests PRIVATE ASSET_PATH="${CMAKE_CURRENT_SOURCE_DIR}/assets/")
target_compile_definitions(resource PRIVATE SHADER_PATH="${CMAKE_CURRENT_SOURCE_DIR}/modules/shader/")
target_link_libraries(AndromedaTests PRIVATE
    GTest::gtest_main
    core
    resource
    renderer
    scene
    editor
    definitions
)

gtest_discover_tests(AndromedaTests)
