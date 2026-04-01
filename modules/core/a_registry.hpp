#pragma once

/**
 * @file ecs.hpp
 * @brief A minimal and efficient Sparse-Set based Entity Component System (ECS).
 */

#include <unordered_map>
#include <typeindex>
#include <vector>
#include <memory>
#include <bitset>
#include <array>
#include <string>

#include "a_primitives.hpp"
#include "a_components.hpp"
#include "a_component_parser.hpp"

 /**
  * @namespace Andromeda::ECS
  * @brief Contains all classes and structures for Entity management and Component storage.
  */
namespace Andromeda::ECS {

    /** @brief Type alias for Entity identifiers. */
    using Entity = uint32_t;

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
        virtual std::string getTypeName() const = 0;
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
        /** @brief Returns the component type name (compiler-dependent via typeid). */
        std::string getTypeName() const override {
            return typeid(T).name();
        }

        /**
         * @brief Adds a component to an entity.
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

        /** @brief Serializes the pool into JSON (Entities and Components). */
        nlohmann::json serializePool() override {
            nlohmann::json j;
            j["entities"] = m_Entitys;
            j["components"] = m_data;
            return j;
        }

        /** @brief Deserializes data and reconstructs the sparse array at runtime. */
        void deserializePool(const nlohmann::json& data) override {
            m_Entitys = data.at("entities").get<std::vector<Entity>>();
            m_data = data.at("components").get<std::vector<T>>();

            m_sparse.clear();
            if (m_Entitys.empty()) return;

            Entity maxEntity = 0;
            for (Entity e : m_Entitys) {
                if (e > maxEntity) maxEntity = e;
            }

            m_sparse.resize(static_cast<size_t>(maxEntity) + 1, 0xFFFFFFFF);

            for (size_t i = 0; i < m_Entitys.size(); ++i) {
                m_sparse[m_Entitys[i]] = i;
            }
        }

        /**
         * @brief Removes the component associated with an entity.
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

        /** @brief Returns the raw dense vector of entity IDs. */
        const std::vector<Entity>& getEntities() const { return m_Entitys; }
    };

    struct EntityHandle;

    /**
     * @class ComponentRegistry
     * @brief The central coordinator for entities and their component pools.
     */
    class ComponentRegistry {
    public:
        /** @brief Map of type indices to specific component pools. */
        std::unordered_map<std::type_index, std::unique_ptr<IComponentPool>> m_Pools;

        /** @brief Counter for generating unique entity IDs. */
        Entity m_NextID = 0;

        /** @brief Generates a new unique Entity ID. */
        Entity createEntity() { return m_NextID++; }

        /**
         * @brief Retrieves (or creates) the component pool for a specific type.
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
            auto index = std::type_index(typeid(T));
            if (m_Pools.find(index) == m_Pools.end()) {
                return false;
            }
            return static_cast<ComponentPool<T>&>(*m_Pools[index]).has(entity);
        }

        /** @brief Removes an entity from all existing pools. */
        void destroyEntity(Entity entity) {
            for (auto const& [type, pool] : m_Pools) {
                pool->removeEntity(entity);
            }
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

        /** @brief Destroys the entity and removes all its components. */
        void destroy() const { registry->destroyEntity(id); }

        /** @brief Implicit conversion to the raw Entity ID. */
        operator Entity() const { return id; }
    };

    inline EntityHandle ComponentRegistry::createHandle() {
        return { createEntity(), this };
    }
}