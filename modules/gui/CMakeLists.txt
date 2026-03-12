   
find_package(imgui REQUIRED)

add_library(gui STATIC
    viewport.cpp
    navbar.cpp
    gui_renderer.cpp
    device_hierarchy.cpp
    device_browser.cpp
    console.cpp
)
target_compile_definitions(gui PRIVATE ASSET_PATH="${CMAKE_CURRENT_SOURCE_DIR}/assets/")
target_include_directories(gui PUBLIC .)
target_include_directories(gui PUBLIC ${CMAKE_SOURCE_DIR}/bindings/)
target_link_libraries(gui PUBLIC 
    imgui::imgui

    core
    definitions
)