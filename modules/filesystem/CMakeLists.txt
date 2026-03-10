
add_library(filesystem STATIC
    a_filesystem.cpp
)

target_include_directories(filesystem PUBLIC .)

target_link_libraries(filesystem PUBLIC
    definitions
)