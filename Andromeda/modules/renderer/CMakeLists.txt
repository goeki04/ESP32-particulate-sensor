find_package(glm REQUIRED)
find_package(glew REQUIRED)
find_package(SDL3 REQUIRED)
find_package(imgui REQUIRED)
add_library(renderer STATIC
    renderer.cpp
)

target_include_directories(renderer PUBLIC 
    .
    ${CMAKE_BINARY_DIR}/generated/
)
target_compile_definitions(renderer PRIVATE SHADER_PATH="${CMAKE_SOURCE_DIR}/modules/shader/")
target_link_libraries(renderer PUBLIC 
    glm::glm
    imgui::imgui

    definitions
    scene
    core
    renderer_rhi
    resource
)