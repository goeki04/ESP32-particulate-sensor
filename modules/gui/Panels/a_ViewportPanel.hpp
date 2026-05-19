#pragma once
#include "a_EditorPanel.hpp"
#include "imgui.h"
#include "ImGuizmo.h"
#include "a_registry.hpp"
#include <array>
namespace Andromeda::Gui {
    struct EditorContext;
    struct TransformIcons {
        enum Type { Translate, Scale, Rotate, Select, Count };

        std::array<ImTextureID,Count> handles;

        static constexpr const char* toolNames[Count] = {
            "translate", "scale", "rotate", "select"
        };

        static constexpr std::optional<ImGuizmo::OPERATION> getImGuizmoTool(const Type tool) {
            switch(tool) {
            case Translate: return ImGuizmo::TRANSLATE;
            case Scale:     return ImGuizmo::SCALE;
            case Rotate:    return ImGuizmo::ROTATE;
            default:
                return std::nullopt;
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

        void updateGizmos(EditorContext& ctx);

        static void prepareImGuizmo(EditorContext& ctx);

        void handleGizmoInteraction(ECS::Component::Transform& transform, mat4& deltaMatrix);

        static void applyGizmoTransform(ECS::Component::Transform& transform,mat4& matrix);

        void finalizeGizmoInteraction(ECS::Component::Transform& currentTransform);

        static void setOverlayStyle();

        static ImGuiWindowFlags setOverlayFlags();

        static void resetOverlayStyle();

        void drawTransformButtons(const TransformIcons& textureHandles, ImGuiWindowFlags flags);

        void drawWireframeControl(const u32& textureID, ImGuiWindowFlags flags);

        void drawViewportOverlay(ImVec2 rectMax, ImVec2 rectMin, const u32& textureID, const TransformIcons& textureHandles);

        static void updateImGuiMousePos(const ViewportDimension& vpDimension, EditorContext& ctx, const ViewportDrawInfo& drawInfo);

        void drawViewportImage(EditorContext& ctx, const ViewportDrawInfo& drawInfo);

        void handleViewportInput(const ViewportDrawInfo& drawInfo);

    private:
        ECS::EntityHandle m_SelectedEntity;
        TransformIcons m_TextureHandles = {};
        bool m_WireframeEnabled = false;
        bool m_IsDraggingGizmo = false;
        std::any m_ActiveUndoState;

    };
}