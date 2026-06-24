#pragma once

/**
 * @file a_guiTypes.hpp
 * @brief Shared lightweight types used to pass rendering/window state between the engine and the ImGui-based editor GUI.
 */

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
     * @details Filled in by the renderer after a scene has been drawn into an offscreen
     *          framebuffer, then consumed by the viewport panel to display the result
     *          as an ImGui image and to map mouse coordinates back into the scene.
     */
    struct ViewportDrawInfo {
        /** @brief Constructs an empty draw info with no camera and a zero-sized framebuffer. */
        ViewportDrawInfo() : camData(nullptr), postProcessingFboTexture(0), framebufferSize(0.0f, 0.0f){}

        amath::CameraData* camData;               ///< Non-owning pointer to the camera that produced this frame.
        Texture postProcessingFboTexture;         ///< RHI handle for the final, post-processed scene color texture.
        vec2 framebufferSize;                     ///< Dimensions, in pixels, of the rendered framebuffer/viewport.
    };
}