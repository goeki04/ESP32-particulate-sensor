find_package(SDL3 REQUIRED)
find_package(stb REQUIRED)
find_package(glew REQUIRED)
find_package(imgui REQUIRED)
find_package(spdlog REQUIRED)
add_library(core STATIC
        "a_subsystem_manager.cpp"
        "window_manager.cpp"
    )

target_include_directories(core PUBLIC .)
target_compile_definitions(core PRIVATE ASSET_PATH="${CMAKE_SOURCE_DIR}/assets/")
target_link_libraries(core PUBLIC
    sdl::sdl
    stb::stb
    GLEW::GLEW
    spdlog::spdlog

    definitions
)