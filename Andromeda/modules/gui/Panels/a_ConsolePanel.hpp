#pragma once
#include "Console.h"
#include "a_EditorPanel.hpp"
namespace Andromeda::Gui {
	class ConsolePanel : public EditorPanel {
    public:
        explicit ConsolePanel(const char* name)
            : EditorPanel(name)
        {
            m_IsOpen = false;
        }
        void onGuiRender(EditorContext& ctx) override;
    private:
        Console::AppConsole m_Console;
	};
}