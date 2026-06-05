find_package(glew REQUIRED)
find_package(SDL3 REQUIRED)
find_package(imgui REQUIRED)
find_package(imguizmo REQUIRED)
find_package(implot REQUIRED)
add_library(gui STATIC
    Framework/gui_renderer.cpp
    Framework/a_Docking.cpp
    Panels/a_HierarchyPanel.cpp
    Panels/a_Chart.cpp
    Panels/a_BrowserPanel.cpp
    Panels/a_DetailsPanel.cpp
    Panels/a_ConsolePanel.cpp
    Panels/a_ViewportPanel.cpp
    Widgets/a_MainMenuBar.cpp
    Widgets/Console.cpp
    ${CMAKE_SOURCE_DIR}/bindings/imgui_impl_sdl3.cpp
    ${CMAKE_SOURCE_DIR}/bindings/imgui_impl_opengl3.cpp
)
target_compile_definitions(gui PRIVATE ASSET_PATH="${CMAKE_SOURCE_DIR}/assets/")
target_include_directories(gui PUBLIC 
    .
    Components
    Framework
    Panels
    State
    Widgets
    ${CMAKE_SOURCE_DIR}/bindings/
    ${CMAKE_BINARY_DIR}/generated/
)
target_compile_definitions(gui PRIVATE IMGUI_IMPL_OPENGL_LOADER_CUSTOM)
target_link_libraries(gui PUBLIC
    GLEW::GLEW
    imgui::imgui
    imguizmo::imguizmo
    sdl::sdl
    implot::implot
    
    core
    scene
    serialization
    definitions
    resource
    renderer_rhi
)