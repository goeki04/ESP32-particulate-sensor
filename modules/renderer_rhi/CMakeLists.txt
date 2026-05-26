find_package(glew REQUIRED)
find_package(imgui REQUIRED)
find_package(glm REQUIRED)
find_package(SDL3 REQUIRED)
add_library(renderer_rhi STATIC
    
    OpenGL/a_opengl_framebuffer.cpp
    OpenGL/a_opengl_upload.cpp
    OpenGL/a_OpenGLContext.cpp
    OpenGL/a_opengl_constant_buffer.cpp
    a_shader_reflection.cpp
    )

include(${CMAKE_SOURCE_DIR}/cmake/CompileShaders.cmake)

file(GLOB_RECURSE RHI_GLSL_SHADERS 
     CONFIGURE_DEPENDS 
     "${CMAKE_SOURCE_DIR}/modules/shader/*.vert"
     "${CMAKE_SOURCE_DIR}/modules/shader/*.frag"
)

compile_shaders(
    TARGET   renderer_rhi
    SHADERS  ${RHI_GLSL_SHADERS}
    OUT_DIR  "${CMAKE_BINARY_DIR}/shaders/spirv"
)

target_include_directories(renderer_rhi PUBLIC 
    .
    ${CMAKE_SOURCE_DIR}/bindings/
)
target_link_libraries(renderer_rhi PUBLIC 
    sdl::sdl
    GLEW::GLEW
    glm::glm
    spirv-reflect-static

    definitions
)