#pragma once
#include "a_EditorPanel.hpp"
#include <string_view>
#include "a_registry.hpp"
#include <any>
namespace Andromeda::Gui {
    struct EditorContext;

	class DetailsPanel : public EditorPanel {
    public:
        explicit DetailsPanel(const char* name)
            : EditorPanel(name)
        {
        }
        void onGuiRender(EditorContext& ctx) override;
        static bool passesFilter(std::string_view name, const char* filter);
        static void renderComponentList(ECS::EntityHandle handle, const char* filter);
        static void renderComponentSearchPopup(const ECS::EntityHandle handle, const float width);
        void drawAddComponentButton(const ECS::EntityHandle handle);
    private:
        std::any m_ActiveUndoState;
	};
}