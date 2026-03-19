#pragma once
#include "a_primitives.hpp"

// Forward Declarations
struct SDL_Window;
namespace Andromeda::amath { struct CameraData; }
namespace Andromeda { class IDeviceProvider; }
namespace Andromeda::ECS { class ComponentRegistry; }

namespace Andromeda::Gui {

    /**
     * @brief Defines the screen alignment for UI elements.
     */
    enum Alignment {
        TopLeft,
        TopRight,
        Center,
        CenterBottom,
        CenterTop,
        BottomLeft,
        BottomRight,
    };

    /**
     * @brief Structure to define margins or padding for layout elements.
     */
    struct Margin {
        float left, right, bottom, top;

        /// Default constructor: initializes all sides to 0.0f.
        Margin() : left(0.0f), right(0.0f), top(0.0f), bottom(0.0f) {}

        /// Sets a uniform margin for all four sides.
        Margin(float margin) : left(margin), right(margin), top(margin), bottom(margin) {}

        /// Sets individual values for each side.
        Margin(float left, float right, float bottom, float top)
            : left(left), right(right), bottom(bottom), top(top) {
        }
    };

    /**
     * @brief Initialization configuration for the GuiRenderer subsystem.
     */
    struct GuiRendererConfig {
        GuiRendererConfig() : window(nullptr), cam(nullptr), registry(nullptr),
            dp(nullptr), sdl_gl_context(nullptr), glsl_version(nullptr) {
        }

        SDL_Window* window;               // Handle to the main SDL window
        amath::CameraData* cam;           // Pointer to the editor camera data
        ECS::ComponentRegistry* registry; // Pointer to the ECS registry
        IDeviceProvider* dp;              // Interface for resource and device management
        void* sdl_gl_context;             // OpenGL context handle
        const char* glsl_version;         // Version string for shader compatibility
    };

    /**
     * @brief Data structure for passing rendering results to the ImGui viewport.
     */
    struct ViewportDrawInfo {
        ViewportDrawInfo() : camData(nullptr), postProcessingFboTexture(0), framebufferSize(0.0f, 0.0f){}

        amath::CameraData* camData;      // Source camera for this viewport
        u32 postProcessingFboTexture;         // OpenGL handle for the final scene texture (post processing texture)
        vec2 framebufferSize;           // Dimensions of the viewport in pixels
    };
}