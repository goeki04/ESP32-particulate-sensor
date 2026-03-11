find_package(glm REQUIRED)
find_package(glew REQUIRED)
find_package(SDL3 REQUIRED)
find_package(imgui REQUIRED)
add_library(renderer STATIC
    renderer.cpp
    camera.cpp
)

target_include_directories(renderer PUBLIC .)

target_link_libraries(renderer PUBLIC 
    glm::glm
    GLEW::GLEW
    sdl::sdl
    imgui::imgui

    definitions
    core
    resource
)