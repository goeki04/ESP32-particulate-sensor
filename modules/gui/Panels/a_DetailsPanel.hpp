#pragma once
#include "a_EditorPanel.hpp"
#include <string_view>
#include "a_registry.hpp"

namespace Andromeda::Gui {
    struct EditorContext;

	class DetailsPanel : public EditorPanel {
    public:
        explicit DetailsPanel(const char* name)
            : EditorPanel(name)
        {
        }
        void onGuiRender(EditorContext& ctx) override;
        bool passesFilter(std::string_view name, const char* filter);
        void renderComponentList(ECS::EntityHandle handle, const char* filter);
        void renderComponentSearchPopup(const ECS::EntityHandle handle, const float width);
        void drawAddComponentButton(const ECS::EntityHandle handle);
	};
}