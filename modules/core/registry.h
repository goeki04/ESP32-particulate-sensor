#pragma once
#include <unordered_map>
#include <typeindex>
#include <vector>
#include <memory>
#include "subsystem.h"

namespace Andromeda::ECS {
    using Entity = uint32_t;
    class IComponentPool {
    public:
        virtual ~IComponentPool() = default;
        virtual void removeEntity(Entity id) = 0;
    };
    template<typename T>
    class ComponentPool : public IComponentPool {
    private:
        std::vector<T> m_data;
        std::vector<Entity> m_Entitys;
        std::vector<size_t> m_sparse;
    public:
        void add(Entity entity, T component) {
            if (entity >= m_sparse.size()) {
                m_sparse.resize(static_cast<size_t>(entity) + 1, 0xFFFFFFFF);
            }
            m_sparse[entity] = m_data.size();
            m_Entitys.push_back(entity);
            m_data.push_back(component);
        }

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

        T& get(Entity entity) { return m_data[m_sparse[entity]]; }
        bool has(Entity entity) const {
            return entity < m_sparse.size() && m_sparse[entity] != 0xFFFFFFFF;
        }
        std::vector<T>& data() { return m_data; }
        const std::vector<Entity>& getEntities() const { return m_Entitys; }
    };

    struct EntityHandle;
    class ComponentRegistry : public ISubsystem {
    private:
        std::unordered_map<std::type_index, std::unique_ptr<IComponentPool>> m_Pools;
        Entity m_NextID = 0;
    public:
        Entity createEntity() { return m_NextID++; }

        template<typename T>
        ComponentPool<T>& getPool() {
            auto index = std::type_index(typeid(T));
            if (m_Pools.find(index) == m_Pools.end()) {
                m_Pools[index] = std::make_unique<ComponentPool<T>>();
            }
            return static_cast<ComponentPool<T>&>(*m_Pools[index]);
        }

        template<typename T>
        void addComponent(Entity entity, T component) {
            getPool<T>().add(entity, component);
        }

        void destroyEntity(Entity entity) {
            for (auto const& [type, pool] : m_Pools) {
                pool->removeEntity(entity);
            }
        }
        EntityHandle createHandle();

        void start() override {}
    };

    struct EntityHandle {
        Entity id;
        ComponentRegistry* registry;

        template<typename T> void add(T component) { registry->addComponent<T>(id, component); }
        template<typename T> T& get() { return registry->getPool<T>().get(id); }
        template<typename T> bool has() { return registry->getPool<T>().has(id); }
        void destroy() { registry->destroyEntity(id); }
        operator Entity() const { return id; }
    };
    inline EntityHandle ComponentRegistry::createHandle() {
        return { createEntity(), this };
    }
} 