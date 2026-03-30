#pragma once

/**
 * @file ecs.hpp
 * @brief A minimal and efficient Sparse-Set based Entity Component System.
 */

#include <unordered_map>
#include <typeindex>
#include <vector>
#include <memory>
#include "a_primitives.hpp"
#include "components.hpp"
#include <bitset>

 /**
  * @namespace Andromeda::ECS
  * @brief Contains all classes and structures for Entity management and Component storage.
  */
namespace Andromeda::ECS {

    /** @brief Type alias for Entity identifiers. */
    using Entity = uint32_t;

    /** @brief Maximum number of unique component types supported by the bitmask. */
    static inline const u32 MAX_COMPONENT_SIZE = 128;

    /**
     * @class IComponentPool
     * @brief Interface for component pools to allow polymorphic storage and entity removal.
     */
    class IComponentPool {
    public:
        virtual ~IComponentPool() = default;
        /** @brief Removes an entity's component data from the pool. */
        virtual void removeEntity(Entity id) = 0;
    };

    /**
     * @class ComponentPool
     * @tparam T The component type managed by this pool.
     * @brief Dense-storage pool for components using a Sparse-Set approach for O(1) access and removal.
     */
    template<typename T>
    class ComponentPool : public IComponentPool {
    private:
        std::vector<T> m_data;          /**< Dense array of component data. */
        std::vector<Entity> m_Entitys;  /**< Dense array of entity IDs corresponding to m_data. */
        std::vector<size_t> m_sparse;   /**< Sparse array mapping Entity ID to index in dense arrays. */

    public:
        /** * @brief Adds a component to an entity.
         * @param entity The target entity ID.
         * @param component The component data to store.
         */
        void add(Entity entity, T component) {
            if (entity >= m_sparse.size()) {
                m_sparse.resize(static_cast<size_t>(entity) + 1, 0xFFFFFFFF);
            }
            m_sparse[entity] = m_data.size();
            m_Entitys.push_back(entity);
            m_data.push_back(component);
        }

        /** * @brief Removes the component associated with an entity.
         * @details Uses the "swap-and-pop" idiom to maintain a dense data array.
         */
        void removeEntity(Entity entity) override {
            if (entity >= m_sparse.size() || m_sparse[entity] == 0xFFFFFFFF) return;

            size_t indexToRemove = m_sparse[entity];
            size_t lastIndex = m_data.size() - 1;
            Entity lastEntity = m_Entitys[lastIndex];

            m_data[indexToRemove] = std::move(m_data[lastIndex]);
            m_Entitys[indexToRemove] = lastEntity;
            m_sparse[lastEntity] = indexToRemove;
            m_sparse[entity] = 0xFFFFFFFF;

            m_data.pop_back();
            m_Entitys.pop_back();
        }

        /** @brief Returns a reference to the component of the given entity. */
        T& get(Entity entity) { return m_data[m_sparse[entity]]; }

        /** @brief Checks if the entity has a component in this pool. */
        bool has(Entity entity) const {
            return entity < m_sparse.size() && m_sparse[entity] != 0xFFFFFFFF;
        }

        /** @brief Returns the raw dense vector of components. */
        std::vector<T>& data() { return m_data; }

        /** @brief Returns the raw dense vector of entities. */
        const std::vector<Entity>& getEntities() const { return m_Entitys; }
    };

    struct EntityHandle;

    /**
     * @class ComponentRegistry
     * @brief The central coordinator for entities and their component pools.
     */
    class ComponentRegistry {
    private:
        std::unordered_map<std::type_index, std::unique_ptr<IComponentPool>> m_Pools; /**< Map of type indices to specific component pools. */
        Entity m_NextID = 0; /**< Counter for generating unique entity IDs. */

    public:
        /** @brief Stores bitmasks for each entity to track which components are active. */
        std::unordered_map<Entity, std::bitset<MAX_COMPONENT_SIZE>> m_EntityComponentMask;

        /** @brief Generates a new unique Entity ID. */
        Entity createEntity() { return m_NextID++; }

        /** * @brief Retrieves (or creates) the component pool for a specific type.
         * @tparam T The component type.
         */
        template<typename T>
        ComponentPool<T>& getPool() {
            auto index = std::type_index(typeid(T));
            if (m_Pools.find(index) == m_Pools.end()) {
                m_Pools[index] = std::make_unique<ComponentPool<T>>();
            }
            return static_cast<ComponentPool<T>&>(*m_Pools[index]);
        }

        /** * @brief Adds a component to an entity and updates its component mask.
         * @tparam T The component type.
         */
        template<typename T>
        void addComponent(Entity entity, T component) {
            u32 typeID = Component::ComponentID::template value<T>();
            m_EntityComponentMask[entity].set(typeID);
            getPool<T>().add(entity, component);
        }

        /** * @brief Checks if an entity has a specific component type.
         * @tparam T The component type.
         */
        template<typename T>
        bool hasComponent(Entity entity) {
            auto index = std::type_index(typeid(T));
            if (m_Pools.find(index) == m_Pools.end()) {
                return false;
            }
            return static_cast<ComponentPool<T>&>(*m_Pools[index]).has(entity);
        }

        /** * @brief Removes an entity from all pools and erases its mask.
         */
        void destroyEntity(Entity entity) {
            for (auto const& [type, pool] : m_Pools) {
                pool->removeEntity(entity);
            }
            m_EntityComponentMask.erase(entity);
        }

        /** @brief Creates a convenient handle for an entity. */
        EntityHandle createHandle();
    };

    /**
     * @struct EntityHandle
     * @brief A wrapper around an Entity ID and Registry for easier component manipulation.
     */
    struct EntityHandle {
        Entity id;                  /**< The underlying Entity ID. */
        ComponentRegistry* registry; /**< Pointer to the associated registry. */

        /** @brief Adds a component to this entity. */
        template<typename T> void add(T component) { registry->addComponent<T>(id, component); }

        /** @brief Gets a reference to a component from this entity. */
        template<typename T> T& get() { return registry->getPool<T>().get(id); }

        /** @brief Checks if this entity has a specific component. */
        template<typename T> bool has() { return registry->getPool<T>().has(id); }

        /** @brief Returns the bitmask of all components attached to this entity. */
        std::bitset<MAX_COMPONENT_SIZE>& getComponentBitmask() { return registry->m_EntityComponentMask[id]; };

        /** @brief Destroys the entity and removes all its components. */
        void destroy() { registry->destroyEntity(id); }

        /** @brief Implicit conversion to the raw Entity ID. */
        operator Entity() const { return id; }
    };

    inline EntityHandle ComponentRegistry::createHandle() {
        return { createEntity(), this };
    }
}