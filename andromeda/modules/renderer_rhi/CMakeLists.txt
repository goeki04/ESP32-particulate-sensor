find_package(imgui REQUIRED)
find_package(glm REQUIRED)
find_package(SDL3 REQUIRED)

# Sources/dependencies for the currently selected ANDROMEDA_GRAPHICS_API backend (set in the
# top-level CMakeLists.txt). Every RHI wrapper (RHIConstantBuffer, RHIFramebuffer, ...) uses the
# Pimpl idiom: its public header is backend-agnostic, and exactly one backend's .cpp below
# provides the implementation, chosen at configure time - not at runtime. That's what lets those
# wrappers stay free of `virtual` without leaking GL/Vulkan/DirectX headers into consumers.
set(RENDERER_RHI_BACKEND_SOURCES "")
set(RENDERER_RHI_BACKEND_LIBRARIES "")

if(ANDROMEDA_GRAPHICS_API STREQUAL "OpenGL")
    find_package(glew REQUIRED)

    set(RENDERER_RHI_BACKEND_SOURCES
        OpenGL/a_opengl_framebuffer.cpp
        OpenGL/a_opengl_upload.cpp
        OpenGL/a_OpenGLContext.cpp
        OpenGL/a_opengl_constant_buffer.cpp
        OpenGL/a_opengl_storage_buffer.cpp
    )
    set(RENDERER_RHI_BACKEND_LIBRARIES GLEW::GLEW)

elseif(ANDROMEDA_GRAPHICS_API STREQUAL "Vulkan")
    message(FATAL_ERROR
        "ANDROMEDA_GRAPHICS_API=Vulkan was selected, but modules/renderer_rhi has no Vulkan "
        "backend yet (only modules/renderer_rhi/OpenGL/ is implemented). Add "
        "modules/renderer_rhi/Vulkan/*.cpp implementing ConstantBufferImpl/FramebufferImpl/etc. "
        "and list them here before building with Vulkan.")

elseif(ANDROMEDA_GRAPHICS_API STREQUAL "DirectX")
    message(FATAL_ERROR
        "ANDROMEDA_GRAPHICS_API=DirectX was selected, but modules/renderer_rhi has no DirectX "
        "backend yet (only modules/renderer_rhi/OpenGL/ is implemented). Add "
        "modules/renderer_rhi/DirectX/*.cpp implementing ConstantBufferImpl/FramebufferImpl/etc. "
        "and list them here before building with DirectX.")

else()
    message(FATAL_ERROR "Unknown ANDROMEDA_GRAPHICS_API '${ANDROMEDA_GRAPHICS_API}'. Valid values: OpenGL, Vulkan, DirectX.")
endif()

add_library(renderer_rhi STATIC
    ${RENDERER_RHI_BACKEND_SOURCES}
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
    ${RENDERER_RHI_BACKEND_LIBRARIES}
    glm::glm
    spirv-reflect-static

    definitions
)