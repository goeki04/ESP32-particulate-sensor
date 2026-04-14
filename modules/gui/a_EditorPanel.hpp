#pragma once
#include "a_guiTypes.hpp"

namespace Andromeda::Gui
{
    struct EditorContext;
}

namespace Andromeda::Gui
{
    /**
         * @brief Abstract base class for all editor UI components.
         * * This interface defines the lifecycle of a UI panel. By inheriting from
         * EditorPanel, any component can be managed and rendered by the GuiRenderer
         * without the renderer needing to know the panel's specific implementation.
         */
    class EditorPanel {

    public:
        /**
         * @brief Virtual destructor to ensure proper cleanup of derived classes.
         */
        virtual ~EditorPanel() = default;

        /**
         * @brief Pure virtual function to handle ImGui rendering logic.
         * * This must be implemented by every panel. It contains the
         * ImGui::Begin() and ImGui::End() calls and all widgets in between.
         */
        virtual void onImGuiRender(EditorContext& ctx) = 0;
        const char* getName() const { return m_Name; }
        explicit EditorPanel(const char* name) : m_Name(name){}
        bool m_IsOpen = true; ///< Controls the visibility of the panel.
        EditorWindowFlags m_WindowFlags = 0;
    protected:
        const char* m_Name = nullptr; ///< The title of the ImGui window.
    };

}
