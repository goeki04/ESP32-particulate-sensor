#pragma once

#include "a_EditorPanel.hpp"
#include <string>
#include "a_primitives.hpp"

/**
 * @namespace Andromeda::ECS
 * @brief Forward declaration of the ECS subsystem components.
 */
namespace Andromeda::ECS
{
    struct EntityHandle;
}

/**
 * @namespace Andromeda::Gui
 * @brief Contains all editor panels and UI rendering logic.
 */
namespace Andromeda::Gui {

    struct EditorContext;

    /**
     * @class HierarchyPanel
     * @brief Displays and manages the scene graph, allowing entity selection, renaming, and spawning.
     * * Inherits from EditorPanel. This panel reads from the central ECS registry to render all
     * active entities, provides case-insensitive search filtering, an inline renaming workflow,
     * and specialized context menus for entity destruction and primitive spawning.
     */
    class HierarchyPanel : public EditorPanel {
    public:
        /**
         * @brief Constructs the HierarchyPanel with a specific identifier name.
         * @param name The display title of the panel window.
         */
        explicit HierarchyPanel(const char* name)
            : EditorPanel(name)
        {
        }

        /**
         * @brief Utility to safely extract an entity's name.
         * @param handle The EntityHandle wrapper of the target entity.
         * @return The entity's Tag component name string, or "Unnamed" if no Tag component is found.
         */
        static const std::string& getEntityName(ECS::EntityHandle& handle);

        /**
         * @brief Handles selection logic when an entity node is clicked.
         * @param ctx The global EditorContext state.
         * @param entity The raw ECS Entity ID to select.
         */
        static void selectEntity(EditorContext& ctx, ECS::Entity entity);

        /**
         * @brief Main node drawing controller for a single entity line item.
         * * Determines whether to render a standard selectable node or redirect to the
         * inline renaming input field based on the current panel state.
         * * @param ctx The global EditorContext state.
         * @param e The specific entity to evaluate and render.
         */
        void drawEntityNode(EditorContext& ctx, ECS::Entity e);

        /**
         * @brief Renders the entire Hierarchy Panel window frame.
         * * Implements the pure virtual interface from EditorPanel. Dispatches the search bar,
         * populates the filtered entity list within a clip-optimized child window, and captures
         * canvas-wide context menu requests.
         * * @param ctx The global EditorContext state.
         */
        void onGuiRender(EditorContext& ctx) override;

        /**
         * @brief Runs once upon engine initialization to cache persistent resources.
         * * Fetches necessary UI asset pointers (e.g., the search magnifying glass icon)
         * from the ResourceManager to eliminate frame-by-frame lookup overhead.
         * * @param ctx The global EditorContext state.
         */
        void initPanel(EditorContext& ctx) override;

    private:
        void* m_SearchTextureHandle = nullptr;     /**< Cached OpenGL texture ID cast to void* for ImGui rendering. */
        char m_RenameBuffer[128] = "";             /**< C-string staging buffer tracking active keyboard input while renaming. */
        char m_SearchQuery[128] = "";              /**< C-string tracking the active character input inside the search bar. */

        /** * @brief Tracks the entity currently entering the inline editing state.
         * Settles to INVALID_ENTITY_ID if no inline editing is happening.
         */
        ECS::Entity m_RenamingEntity = ECS::INVALID_ENTITY_ID;
        bool m_FocusRenameInput = false;           /**< Flag triggering ImGui auto-focus on the text input block for a single frame. */

        /**
         * @brief Renders a padding-less text input field over an active entity node to handle inline editing.
         * @param handle The EntityHandle of the entity being manipulated.
         */
        void drawRenamingInput(ECS::EntityHandle handle);

        /**
         * @brief Renders the standard highlighted text node and processes double-click shortcuts.
         * @param ctx The global EditorContext state.
         * @param e The raw entity target ID.
         * @param handle The operational handle corresponding to entity @p e.
         * @param isSelected Flags whether this specific entity is actively selected in the editor.
         */
        void drawNormalSelectable(EditorContext& ctx, ECS::Entity e, ECS::EntityHandle handle, bool isSelected);

        /**
         * @brief Spawns the specific contextual context menu overlay bound to an individual entity.
         * * Provides explicit actions like "Delete" or "Rename" that target a chosen instance.
         * * @param ctx The global EditorContext state.
         * @param e The raw entity target ID.
         * @param handle The operational handle corresponding to entity @p e.
         * @param isSelected Flags whether this entity is selected.
         */
        void drawEntityContextMenu(EditorContext& ctx, ECS::Entity e, ECS::EntityHandle handle, bool isSelected);

        /**
         * @brief Spawns the global context menu overlay bound to the empty workspace area.
         * * Provides actions such as spawning default primitives (Cube, Sphere, Plane) or
         * creating empty game objects.
         * * @param ctx The global EditorContext state.
         */
        void drawContextMenu(EditorContext& ctx);

        /**
         * @brief Activates the rename state, populating staging buffers with current asset properties.
         * @param handle The target entity's handle to query and store.
         */
        void beginRename(ECS::EntityHandle handle);
    };
}