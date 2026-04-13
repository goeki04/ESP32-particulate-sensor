#pragma once
#include "a_EditorPanel.hpp"
#include <string>
#include "a_primitives.hpp"
namespace Andromeda::ECS
{
    struct EntityHandle;
}
namespace Andromeda::Gui{
    struct EditorContext;

    class HierarchyPanel : public EditorPanel{
    public:
        EditorContext& m_Context;
        static std::string getEntityName(ECS::EntityHandle& handle);
        void selectEntity(EditorContext& ctx, ECS::Entity entity);
        void drawEntityNode(EditorContext& ctx, ECS::Entity e);
        explicit HierarchyPanel(EditorContext& ctx) : m_Context(ctx) {}
        void onImGuiRender(EditorContext& ctx) override;
    };
}