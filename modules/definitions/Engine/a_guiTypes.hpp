#pragma once
#include "a_primitives.hpp"
#include "a_texture.hpp"
namespace Andromeda::amath { struct CameraData; }

namespace Andromeda::Gui {
     /**
     * @brief Type alias for ImGui window configuration flags.
     * * This typedef allows the EditorPanel interface to store window flags (like
     * ImGuiWindowFlags) without requiring the full 'imgui.h' header in this file.
     */
    typedef i32 EditorWindowFlags;

    /**
     * @brief Data structure for passing rendering results to the ImGui viewport.
     */
    struct ViewportDrawInfo {
        ViewportDrawInfo() : camData(nullptr), postProcessingFboTexture(0), framebufferSize(0.0f, 0.0f){}

        amath::CameraData* camData;      // Source camera for this viewport
        Texture postProcessingFboTexture;         // OpenGL handle for the final scene texture (post-processing texture)
        vec2 framebufferSize;           // Dimensions of the viewport in pixels
    };
}