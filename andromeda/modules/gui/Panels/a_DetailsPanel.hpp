#pragma once

/**
 * @file a_DetailsPanel.hpp
 * @brief Inspector panel that displays and edits the components of the selected entity.
 */

#include "a_EditorPanel.hpp"
#include <string_view>
#include "a_registry.hpp"
#include <any>
namespace Andromeda::Gui {
    struct EditorContext;

    /**
     * @class DetailsPanel
     * @brief The "Inspector": lists every component on the selected entity and exposes editable widgets.
     * @details Also provides an "Add Component" workflow with a searchable popup. Edits are routed through
     *          the undo system via @c m_ActiveUndoState (snapshot taken before a change is committed).
     */
	class DetailsPanel : public EditorPanel {
    public:
        /**
         * @brief Constructs the Details (Inspector) panel.
         * @param name The display title of the panel window.
         */
        explicit DetailsPanel(const char* name)
            : EditorPanel(name)
        {
        }

        /** @copydoc EditorPanel::onGuiRender */
        void onGuiRender(EditorContext& ctx) override;

        /**
         * @brief Case-insensitive substring test used to filter the component search list.
         * @param name The candidate component name.
         * @param filter The current search query (null/empty matches everything).
         * @return true if @p name matches @p filter.
         */
        static bool passesFilter(std::string_view name, const char* filter);

        /**
         * @brief Renders the collapsible UI for every component currently attached to the entity.
         * @param handle Handle to the entity being inspected.
         * @param filter Optional search filter applied to component names.
         */
        static void renderComponentList(ECS::EntityHandle handle, const char* filter);

        /**
         * @brief Renders the searchable popup listing components that can be added to the entity.
         * @param handle Handle to the target entity.
         * @param width Desired width of the popup, in pixels.
         */
        static void renderComponentSearchPopup(const ECS::EntityHandle handle, const float width);

        /**
         * @brief Draws the "Add Component" button that opens the component search popup.
         * @param handle Handle to the target entity.
         */
        void drawAddComponentButton(const ECS::EntityHandle handle);
    private:
        std::any m_ActiveUndoState; ///< Snapshot of a component captured before an edit, for undo support.
	};
}