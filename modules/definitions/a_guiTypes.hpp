#pragma once
namespace Andromeda::Gui {
    enum Alignment {
        TopLeft,
        TopRight,
        Center,
        CenterBottom,
        CenterTop,
        BottomLeft,
        BottomRight,
    };

    struct Margin {
        float left;
        float right;
        float bottom;
        float top;
        Margin() {
            left = 0;
            right = 0;
            top = 0;
            bottom = 0;
        }
        Margin(float margin) : left(margin), right(margin), top(margin), bottom(margin) {
        }
        Margin(float left, float right, float bottom, float top)
            : left(left), right(right), bottom(bottom), top(top) {
        }
    };
    
    struct GuiRendererConfig {
       SDL_Window* window;
       amath::CameraData* cam;
       ECS::ComponentRegistry* registry;
       void* sdl_gl_context;
       const char* glsl_version;
    };
    //amath::CameraData& camData, GuiRenderer& guiRenderer, u32 framebufferTexture, vec2 framebufferSize, float* ImGuiMouseX, float* ImGuiMouseY
    struct ViewportDrawInfo {
        amath::CameraData& camData;
        u32 framebufferTexture;
        vec2 framebufferSize;
        float* ImGuiMouseX;
        float* ImGuiMouseY;
    };
}