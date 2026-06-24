#pragma once

/**
 * @file a_ConsolePanel.hpp
 * @brief Editor panel hosting the interactive in-engine command console.
 */

#include "Console.h"
#include "a_EditorPanel.hpp"
namespace Andromeda::Gui {
    /**
     * @class ConsolePanel
     * @brief Wraps an @c AppConsole in an editor panel, providing a command input and log output view.
     * @details Hidden by default (@c m_IsOpen is false); typically toggled via the "Window" menu.
     */
	class ConsolePanel : public EditorPanel {
    public:
        /**
         * @brief Constructs the Console panel (initially hidden).
         * @param name The display title of the panel window.
         */
        explicit ConsolePanel(const char* name)
            : EditorPanel(name)
        {
            m_IsOpen = false;
        }

        /** @copydoc EditorPanel::onGuiRender */
        void onGuiRender(EditorContext& ctx) override;
    private:
        Console::AppConsole m_Console; ///< The console state: input buffer, log history and command registry.
	};
}