add_library(editor STATIC
    "editor.cpp"
    "a_command_dispatcher.cpp"
    "a_undo_buffer.cpp"
)

target_include_directories(editor PUBLIC .)
target_include_directories(editor PUBLIC ${CMAKE_SOURCE_DIR}/bindings/)
target_link_libraries(editor PUBLIC
    core
    scene
    renderer_rhi
    input
    renderer
    gui
    resource
)
