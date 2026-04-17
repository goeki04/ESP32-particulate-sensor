#pragma once
#include "a_EditorPanel.hpp"
#include "imgui.h"

namespace Andromeda::Gui {
    struct EditorContext;
    struct TransformIcons {
        enum Type { Translate, Scale, Rotate, Select, Count };

        u32 handles[Count];

        static constexpr const char* toolNames[Count] = {
            "translate", "scale", "rotate", "select"
        };
    };

    struct ViewportDimension {
        ImVec2 min;
        ImVec2 max;
        ImVec2 size;
    };
    class ViewportPanel : public EditorPanel {
    public:

        explicit ViewportPanel(const char* name) : EditorPanel(name) {}

        void onImGuiRender(EditorContext& ctx) override;

        void setOverlayStyle();

        ImGuiWindowFlags setOverlayFlags();

        void resetOverlayStyle();

        void drawTransformButtons(const TransformIcons& textureHandles, const ImGuiWindowFlags flags);

        void drawWireframeControl(const u32& textureID, const ImGuiWindowFlags flags);

        void drawViewportOverlay(ImVec2 rectMax, ImVec2 rectMin, const u32& textureID, const TransformIcons& textureHandles);

        void updateImGuiMousePos(const ViewportDimension& vpDimension, EditorContext& ctx, const Gui::ViewportDrawInfo& drawInfo);

        void drawViewportImage(EditorContext& ctx, const Gui::ViewportDrawInfo& drawInfo);

        void handleViewportInput(const Gui::ViewportDrawInfo& drawInfo);
    };
}