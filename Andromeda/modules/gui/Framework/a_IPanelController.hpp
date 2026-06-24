#pragma once

/**
 * @file a_IPanelController.hpp
 * @brief Interface for querying and toggling the visibility of editor panels by name.
 */

#include <string_view>
#include <vector>
namespace Andromeda::Gui{
    /**
     * @class IPanelController
     * @brief Abstraction allowing UI elements (e.g. the "Window" menu) to control panel
     *        visibility without depending on the concrete @c GuiRenderer.
     *
     * @details Implemented by @c GuiRenderer. Panels are identified by their display name,
     *          which keeps callers like the menu bar decoupled from the panel container.
     */
    class IPanelController{
    public:
        virtual ~IPanelController() = default;

        /**
         * @brief Checks whether the named panel is currently open/visible.
         * @param name The display name of the panel to query.
         * @return true if the panel exists and is open, false otherwise.
         */
        virtual bool isPanelOpen(std::string_view name) const = 0;

        /**
         * @brief Sets the open/visible state of the named panel.
         * @param name The display name of the panel to modify.
         * @param open true to show the panel, false to hide it.
         */
        virtual void setPanelOpen(std::string_view name, bool open) = 0;

        /**
         * @brief Returns the display names of all registered panels.
         * @return A list of panel names, e.g. for populating a "Window" menu.
         */
        virtual std::vector<std::string_view> getPanelNames() const = 0;
    };
}