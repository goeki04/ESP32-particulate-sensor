find_package(glew REQUIRED)
find_package(SDL3 REQUIRED)
find_package(imgui REQUIRED)

add_library(gui STATIC
    viewport.cpp
    navbar.cpp
    details.cpp
    gui_renderer.cpp
    device_hierarchy.cpp
    component_ui.cpp
    device_browser.cpp
    console.cpp
    ${CMAKE_SOURCE_DIR}/bindings/imgui_impl_sdl3.cpp
    ${CMAKE_SOURCE_DIR}/bindings/imgui_impl_opengl3.cpp
)
target_compile_definitions(gui PRIVATE ASSET_PATH="${CMAKE_SOURCE_DIR}/assets/")
target_include_directories(gui PUBLIC .)
target_include_directories(gui PUBLIC ${CMAKE_SOURCE_DIR}/bindings/)
target_compile_definitions(gui PRIVATE IMGUI_IMPL_OPENGL_LOADER_CUSTOM)
target_link_libraries(gui PUBLIC
    GLEW::GLEW
    imgui::imgui
    sdl::sdl

    core
    scene
    serialization
    definitions
    renderer_rhi
)