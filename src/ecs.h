#pragma once
using Entity = uint32_t;
#include <unordered_map>
#include <typeindex>
class IComponentPool {
public:
    virtual ~IComponentPool() = default;
    virtual void removeEntity(Entity id) = 0;
};
//container which stores all the needed data for one component type
template<typename T>
class ComponentPool : public IComponentPool {
private:
    std::vector<T> m_data;
    std::vector<Entity> m_Entitys;
    std::vector<size_t> m_sparse;
public:
    void add(Entity entity, T component) {
        if (entity >= m_sparse.size()) {
            m_sparse.resize(entity + 1, 0xFFFFFFFF); // entity = 0 -> sparseSize = 1
        }
        m_sparse[entity] = m_data.size();
        m_Entitys.push_back(entity);
        m_data.push_back(component);
    }
    //swap and pop
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

    T& get(Entity entity) {
        return m_data[m_sparse[entity]];
    }
    std::vector<T>& data() { return m_data; }
};

class ComponentRegistry {
private:
    std::unordered_map<std::type_index, std::unique_ptr<IComponentPool>> pools;
    Entity nextID = 0;
public:
    Entity createEntity() {
        return nextID++;
    }
    template<typename T>
    ComponentPool<T>& getPool() {
        auto index = std::type_index(typeid(T));
        if (pools.find(index) == pools.end()) {
            pools[index] = std::make_unique<ComponentPool<T>>();
        }
        return static_cast<ComponentPool<T>&>(*pools[index]);
    }
    template<typename T>
    void addComponent(Entity entity, T component) {
        getPool<T>().add(entity, component);
    }
};