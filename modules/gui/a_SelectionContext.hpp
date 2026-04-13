#pragma once
#include "a_primitives.hpp"
namespace Andromeda
{
    /**
  * @brief Manages the entity selection state within the editor.
  * * This structure acts as a central synchronization point for all UI panels.
  * When a user selects an entity in the Hierarchy, the Inspector (Details)
  * panel retrieves the ID from here to display its components.
  */
    struct SelectionContext
    {
        /**
         * @brief Constructs a SelectionContext with no entity selected.
         */
        SelectionContext() : m_SelectedEntity(ECS::INVALID_ENTITY_ID) {}

        /**
         * @brief Updates the currently selected entity.
         * @param entity The entity handle to select. Pass ECS::INVALID_ENTITY_ID to clear selection.
         */
        void setSelectedEntity(const ECS::Entity entity) {
            m_SelectedEntity = entity;
        }

        /**
         * @brief Retrieves the ID of the currently selected entity.
         * @return The ECS::Entity ID, or ECS::INVALID_ENTITY_ID if nothing is selected.
         */
        [[nodiscard]] ECS::Entity getSelectedEntity() const {
            return m_SelectedEntity;
        }

        /**
         * @brief Checks if an entity is currently selected.
         * @return true if a valid entity is selected, false otherwise.
         */
        [[nodiscard]] bool hasSelection() const {
            return m_SelectedEntity != ECS::INVALID_ENTITY_ID;
        }

    private:
        ECS::Entity m_SelectedEntity; ///< The ID of the currently active entity.
    };
}
