#pragma once
#include "a_EditorPanel.hpp"
namespace Andromeda::Gui {
	class BrowserPanel : public EditorPanel {
    public:
        explicit BrowserPanel(const char* name)
            : EditorPanel(name)
        {
        }
        void onImGuiRender(EditorContext& ctx) override;
	};
}