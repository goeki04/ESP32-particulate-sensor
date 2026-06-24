#pragma once

/**
 * @file a_registry.hpp
 * @brief A minimal and efficient Sparse-Set based Entity Component System (ECS).
 */

#include <unordered_map>
#include <typeindex>
#include <vector>
#include <unordered_set>
#include <memory>
#include <limits>
#include <string>
#include "a_component_parser.hpp"
 /**
  * @namespace Andromeda::ECS
  * @brief Contains all classes and structures for Entity management and Component storage.
  */
namespace Andromeda::ECS {

    /**
     * @class IComponentPool
     * @brief Interface for component pools to allow polymorphic storage and serialization.
     */
    class IComponentPool {
    public:
        virtual ~IComponentPool() = default;

        /** @brief Removes an entity's component data from the pool. */
        virtual void removeEntity(Entity id) = 0;

        /** @brief Converts the entire pool into a JSON object. */
        virtual nlohmann::json serializePool() = 0;

        /** @brief Loads pool data from a JSON object and reconstructs the sparse set. */
        virtual void deserializePool(const nlohmann::json& data) = 0;

        /** @brief Returns the name of the stored component type. */
        [[nodiscard]] virtual std::string getTypeName() const = 0;

        /** @brief Returns the list of entities that have this component. */
        [[nodiscard]] virtual const std::vector<Entity>& getEntities() const = 0;
    };
    static constexpr size_t INVALID_INDEX = std::numeric_limits<size_t>::max();
    /**
     * @class ComponentPool
     * @tparam T The component type managed by this pool.
     * @brief Dense-storage pool for components using a Sparse-Set approach for O(1) access and removal.
     */
    template<typename T>
    class ComponentPool : public IComponentPool {

    private:
        std::vector<T> m_data;          /**< Dense array of component data. */
        std::vector<Entity> m_Entities;  /**< Dense array of entity IDs corresponding to m_data. */
        std::vector<size_t> m_sparse;   /**< Sparse array mapping Entity ID to index in dense arrays. */

    public:
        /** @brief Returns the component type name (compiler-dependent via typeid). */
        [[nodiscard]] std::string getTypeName() const override {
            return typeid(T).name();
        }

        /**
         * @brief Adds a component to an entity.
         * @param entity The target entity ID.
         * @param component The component data to store.
         */
        void add(const Entity entity, T component) {
            if (entity >= m_sparse.size()) {
                m_sparse.resize(static_cast<size_t>(entity) + 1, INVALID_INDEX);
            }
            m_sparse[entity] = m_data.size();
            m_Entities.push_back(entity);
            m_data.push_back(component);
        }

        /** @brief Serializes the pool into JSON (Entities and Components). */
        nlohmann::json serializePool() override {
            nlohmann::json j;
            j["entities"] = m_Entities;
            j["components"] = m_data;
            return j;
        }

        /** @brief Deserializes data and reconstructs the sparse array at runtime. */
        void deserializePool(const nlohmann::json& data) override {
            m_Entities = data.at("entities").get<std::vector<Entity>>();
            m_data = data.at("components").get<std::vector<T>>();

            m_sparse.clear();
            if (m_Entities.empty()) return;

            Entity maxEntity = 0;
            for (Entity e : m_Entities) {
                if (e > maxEntity) maxEntity = e;
            }

            m_sparse.resize(static_cast<size_t>(maxEntity) + 1, INVALID_INDEX);

            for (size_t i = 0; i < m_Entities.size(); ++i) {
                m_sparse[m_Entities[i]] = i;
            }
        }

        /**
         * @brief Removes the component associated with an entity.
         * @details Uses the "swap-and-pop" idiom to maintain a dense data array.
         */
        void removeEntity(const Entity entity) override {
            if (entity >= m_sparse.size() || m_sparse[entity] == INVALID_INDEX) return;

            size_t indexToRemove = m_sparse[entity];
            size_t lastIndex = m_data.size() - 1;
            Entity lastEntity = m_Entities[lastIndex];

            m_data[indexToRemove] = std::move(m_data[lastIndex]);
            m_Entities[indexToRemove] = lastEntity;
            m_sparse[lastEntity] = indexToRemove;
            m_sparse[entity] = INVALID_INDEX;

            m_data.pop_back();
            m_Entities.pop_back();
        }

        /** @brief Returns a reference to the component of the given entity. */
        T& get(const Entity entity) { 
            assert(entity < m_sparse.size() && "Entity ID out of sparse bounds!");
            size_t index = m_sparse[entity];
            assert(index < m_data.size() && "Entity index out of dense bounds!");
            return m_data[m_sparse[entity]]; 
        }

        /** @brief Checks if the entity has a component in this pool. */
        [[nodiscard]] bool has(const Entity entity) const {
            return entity < m_sparse.size() && m_sparse[entity] != INVALID_INDEX;
        }

        /** @brief Returns the raw dense vector of components. */
        std::vector<T>& data() { return m_data; }

        /** @brief Returns the raw dense vector of entity IDs. */
        [[nodiscard]] const std::vector<Entity>& getEntities() const { return m_Entities; }
    };

    struct EntityHandle;

    /**
     * @class ComponentRegistry
     * @brief The central coordinator for entities and their component pools.
     */
    class ComponentRegistry {
    public:
        /** @brief List of currently active entities. */
        std::vector<Entity> m_ActiveEntities;

        /** @brief Map of type indices to specific component pools. */
        std::unordered_map<std::type_index, std::unique_ptr<IComponentPool>> m_Pools;

        /** @brief Counter for generating unique entity IDs. */
        Entity m_NextID = 0;
  
        /** @brief Generates a new unique Entity ID. */
        Entity createEntity() { 
            Entity newEntity = m_NextID++;
            m_ActiveEntities.push_back(newEntity);
            return newEntity;
        }

        /**
         * @brief Rebuilds @c m_ActiveEntities from the union of all entities across every pool.
         * @details Useful after deserialization, where pools are restored directly and the active
         *          entity list must be reconstructed to reflect their contents.
         */
        void rebuildActiveEntities() {
            std::unordered_set<Entity> unique;
            for (const auto& pool : m_Pools | std::views::values) {
                for (Entity e : pool->getEntities()) unique.insert(e);
            }
            m_ActiveEntities.assign(unique.begin(), unique.end());
        }

		/** @brief Wipes the registry: clears all entities and component pools and resets the ID counter (e.g. when loading a new scene). */
		void clearRegistry() {
			m_ActiveEntities.clear();
			m_Pools.clear();
			m_NextID = 0;
		}

        /**
         * @brief Retrieves (or creates) the component pool for a specific type.
         * @tparam T The component type.
         */
        template<typename T>
        ComponentPool<T>& getPool() {
            const auto index = std::type_index(typeid(T));
            if (!m_Pools.contains(index)) {
                m_Pools[index] = std::make_unique<ComponentPool<T>>();
            }
            return static_cast<ComponentPool<T>&>(*m_Pools[index]);
        }

        /**
         * @brief Adds a component to an entity.
         * @tparam T The component type.
         */
        template<typename T>
        void addComponent(Entity entity, T component) {
            getPool<T>().add(entity, component);
        }

        /**
         * @brief Checks if an entity has a specific component type.
         * @tparam T The component type.
         */
        template<typename T>
        bool hasComponent(Entity entity) {
            const auto index = std::type_index(typeid(T));
            if (!m_Pools.contains(index)) {
                return false;
            }
            return static_cast<ComponentPool<T>&>(*m_Pools[index]).has(entity);
        }

        /** @brief Removes an entity from all existing pools. */
        void destroyEntity(const Entity entity) {
            for (const auto& pool : m_Pools | std::views::values) {
                pool->removeEntity(entity);
            }

            auto it = std::find(m_ActiveEntities.begin(), m_ActiveEntities.end(), entity);
            if (it != m_ActiveEntities.end()) {
                *it = m_ActiveEntities.back();
                m_ActiveEntities.pop_back();
            }
        }
        /** @brief Returns a reference to the list of all active entities. */
        [[nodiscard]] const std::vector<Entity>& getAllEntities() const {
            return m_ActiveEntities;
        }

        /** @brief Creates a convenient handle for an entity. */
        EntityHandle createHandle();
    };

    /**
     * @struct EntityHandle
     * @brief A wrapper around an Entity ID and Registry for easier component manipulation.
     */
    struct EntityHandle {
        Entity id = ECS::INVALID_ENTITY_ID;                  /**< The underlying Entity ID. */
        ComponentRegistry* registry = nullptr; /**< Pointer to the associated registry. */

        /** @brief Adds a component to this entity. */
        template<typename T> void add(T component) { registry->addComponent<T>(id, component); }

        /** @brief Gets a reference to a component from this entity. */
        template<typename T> T& get() { return registry->getPool<T>().get(id); }

        /** @brief Checks if this entity has a specific component. */
        template<typename T> bool has() { return registry->getPool<T>().has(id); }

        /** @brief Destroys the entity and removes all its components. */
        void destroy() const { registry->destroyEntity(id); }

        /** @brief Implicit conversion to the raw Entity ID. */
        operator Entity() const { return id; }
    };

    inline EntityHandle ComponentRegistry::createHandle() {
        return { createEntity(), this };
    }
}