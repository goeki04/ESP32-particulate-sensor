find_package(glew REQUIRED)
find_package(imgui REQUIRED)
find_package(glm REQUIRED)
find_package(SDL3 REQUIRED)
add_library(renderer_rhi STATIC
    shader.cpp
    OpenGL/a_opengl_framebuffer.cpp
    a_IGraphicsContext.cpp
    
    OpenGL/a_opengl_upload.cpp
    OpenGL/a_OpenGLContext.cpp
    )

target_include_directories(renderer_rhi PUBLIC .)
target_include_directories(renderer_rhi PUBLIC ${CMAKE_SOURCE_DIR}/bindings/)
target_link_libraries(renderer_rhi PUBLIC 
    sdl::sdl
    GLEW::GLEW
    glm::glm

    definitions
)