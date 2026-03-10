
find_package(imgui REQUIRED)

add_library(gui STATIC
    viewport.cpp
    navbar.cpp
    gui_renderer.cpp
    device_hierarchy.cpp
    device_browser.cpp
    console.cpp
)

target_include_directories(gui PUBLIC .)

target_link_libraries(gui PUBLIC 
    imgui::imgui
)