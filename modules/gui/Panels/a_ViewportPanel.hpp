#pragma once
#include "a_EditorPanel.hpp"
#include "imgui.h"
#include "ImGuizmo.h"
#include "a_registry.hpp"
namespace Andromeda::Gui {
    struct EditorContext;
    struct TransformIcons {
        enum Type { Translate, Scale, Rotate, Select, Count };

        u32 handles[Count];

        static constexpr const char* toolNames[Count] = {
            "translate", "scale", "rotate", "select"
        };

        static constexpr ImGuizmo::OPERATION getImGuizmoTool(Type tool) {
            switch(tool) {
                case Translate: return ImGuizmo::TRANSLATE;
                case Scale: return ImGuizmo::SCALE;
                case Rotate: return ImGuizmo::ROTATE;
            }
        }
    };

    struct ViewportDimension {
        ImVec2 min;
        ImVec2 max;
        ImVec2 size;
    };
    class ViewportPanel : public EditorPanel {
    public:
        TransformIcons::Type m_ActiveTool = TransformIcons::Select;
        explicit ViewportPanel(const char* name) : EditorPanel(name), m_SelectedEntity({ECS::INVALID_ENTITY_ID,nullptr}) {}

        void initPanel(EditorContext& ctx) override;

        void onGuiRender(EditorContext& ctx) override;

        void setOverlayStyle();

        ImGuiWindowFlags setOverlayFlags();

        void resetOverlayStyle();

        void drawTransformButtons(const TransformIcons& textureHandles, const ImGuiWindowFlags flags);

        void drawWireframeControl(const u32& textureID, const ImGuiWindowFlags flags);

        void drawViewportOverlay(ImVec2 rectMax, ImVec2 rectMin, const u32& textureID, const TransformIcons& textureHandles);

        void updateImGuiMousePos(const ViewportDimension& vpDimension, EditorContext& ctx, const Gui::ViewportDrawInfo& drawInfo);

        void drawViewportImage(EditorContext& ctx, const Gui::ViewportDrawInfo& drawInfo);

        void handleViewportInput(const Gui::ViewportDrawInfo& drawInfo);

    private:
        ECS::EntityHandle m_SelectedEntity;
        bool m_WireframeEnabled = false;
        bool m_IsDraggingGizmo = false;
        std::any m_ActiveUndoState;
    };
}