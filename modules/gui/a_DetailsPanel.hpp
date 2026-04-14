#pragma once
#include "a_EditorPanel.hpp"
namespace Andromeda::Gui {
	class DetailsPanel : public EditorPanel {
    public:
        explicit DetailsPanel(const char* name)
            : EditorPanel(name)
        {
        }
        void onImGuiRender(EditorContext& ctx) override;
	};
}