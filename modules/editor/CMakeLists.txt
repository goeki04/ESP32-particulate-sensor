add_library(editor STATIC
    "editor.cpp"
)

target_include_directories(editor PUBLIC .)
target_include_directories(editor PUBLIC ${CMAKE_SOURCE_DIR}/bindings/)
target_link_libraries(editor PUBLIC
    core
    scene
    renderer_rhi
    renderer
    gui
    resource
)
