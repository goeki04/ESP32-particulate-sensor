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
        explicit HierarchyPanel(const char* name)
            : EditorPanel(name)
        { }

        static std::string getEntityName(ECS::EntityHandle& handle);
        void selectEntity(EditorContext& ctx, ECS::Entity entity);
        void drawEntityNode(EditorContext& ctx, ECS::Entity e);
        void onImGuiRender(EditorContext& ctx) override;
    };
}