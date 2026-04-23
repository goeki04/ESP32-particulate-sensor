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
        static void selectEntity(EditorContext& ctx, ECS::Entity entity);
        static void drawEntityNode(EditorContext& ctx, ECS::Entity e);
        void onGuiRender(EditorContext& ctx) override;
    };
}