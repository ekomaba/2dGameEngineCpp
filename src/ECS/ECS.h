#ifndef ECS_H
#define ECS_H
#include <bitset>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <typeindex>
#include <set>
#include <memory>
#include "../Logger/Logger.h"

const unsigned int MAX_COMPONENTS = 32;

// Signature represents the set of components an entity has using bitset
typedef std::bitset<MAX_COMPONENTS> Signature;

// Base component class to manage unique IDs for components
struct IComponent {
protected:
    static int nextId;
};

// Provides a unique ID for each component type
template <typename T>
class Component : public IComponent {
public:
    static int GetId() {
        static auto id = nextId++;
        return id;
    }
};

// Entity represents a game object with a unique ID
class Entity {
private:
    int id;

public:
    Entity (int id) : id(id) {}; 
    Entity(const Entity& entity) = default;
    int GetId() const { return id; }

    Entity& operator =(const Entity& other) = default;
    bool operator ==(const Entity& other) const { return id == other.id; }
    bool operator !=(const Entity& other) const { return id != other.id; }
    bool operator >(const Entity& other) const { return id > other.id; }
    bool operator <(const Entity& other) const { return id < other.id; }

    template <typename TComponent, typename ...TArgs> void AddComponent(TArgs&& ...args);
    template <typename TComponent> void RemoveComponent();
    template <typename TComponent> bool HasComponent() const;
    template <typename TComponent> TComponent& GetComponent() const;

    // forward declaration
    class Registry* registry;
};

// Manages entities and generates a unique signature for each component type
class System { 
private:
    Signature componentSignature;
    std::vector<Entity> entities;

public:
    System() = default;
    ~System() = default;
    void AddEntity(Entity entity);
    void RemoveEntity(Entity entity);
    std::vector<Entity> GetSystemEntities() const;
    const Signature& GetComponentSignature() const;
    template<typename TComponent> void RequireComponent();
};


class IPool {
public:
    virtual ~IPool() {};
};

// Pool 
template<typename T>
class Pool : public IPool {
private:
    std::vector<T> data;
public:
    Pool(int size = 100) { data.resize(size); }
    virtual ~Pool() = default;

    bool isEmpty() const { return data.empty(); }
    int GetSize() const { return data.size(); }
    void Resize(int n) { data.resize(n); }
    void Clear() { data.clear(); }
    void Add(T object) { data.push_back(object); }
    void Set(int index, T object) { data[index] = object; }
    T& Get(int index) { return static_cast<T&>(data[index]); }
    T& operator [](unsigned int index) { return data[index]; }
};

// Manages the creation and destruction of entities, add systems and components to entities
class Registry {
private:
    int numEntities = 0;
    std::vector<std::shared_ptr<IPool>> componentPools;
    std::vector<Signature> entityComponentSignatures;
    std::unordered_map<std::type_index, std::shared_ptr<System>> systems;
    std::set<Entity> entitiesToBeAdded;
    std::set<Entity> entitiesToBeKilled;

public:
    Registry();
    ~Registry();

    // Entity management
    Entity CreateEntity();

    // Component management
    template <typename TComponent, typename ...TArgs> void AddComponent(Entity entity, TArgs&& ...args);
    template <typename TComponent> void RemoveComponent(Entity entity);
    template <typename TComponent> bool HasComponent(Entity entity) const;
    template <typename TComponent> TComponent& GetComponent(Entity entity) const;

    // System management
    template <typename TSystem, typename ...TArgs> void AddSystem(TArgs&& ...args);
    template <typename TSystem> void RemoveSystem();
    template <typename TSystem> bool HasSystem() const;
    template <typename TSystem> TSystem& GetSystem() const;

    // Check the component signature of an entity and add the entity to the systems that match
    void AddEntityToSystems(Entity entity);

    // Processes the entities that are waiting to be added/killed
    void Update();

};

// Template function to require a component in a system by setting the appropriate bit in the signature
template<typename TComponent> 
void System::RequireComponent() {
    const auto componentId = Component<TComponent>::GetId(); // Generates a unique ID for each component type
    componentSignature.set(componentId); // Marks the component as required by setting the corresponding bit to true
}

// Template function to add a new component to a ...
template <typename TComponent, typename ...TArgs> 
void Registry::AddComponent(Entity entity, TArgs&& ...args) {
    std::size_t componentId = Component<TComponent>::GetId();
    const auto entityId = entity.GetId();

    // If the component id is bigger then component pool size we will increase the Pool's size
    if (componentId >= componentPools.size()) {
        componentPools.resize(componentId + 1, nullptr);
    }

    // If we don't have the Pool, it will create it
    if (!componentPools[componentId]) {
        std::shared_ptr<Pool<TComponent>> newComponentPool = std::make_shared<Pool<TComponent>>();
        componentPools[componentId] = newComponentPool;
    }

    // Get the pool of component values for that component type
    std::shared_ptr<Pool<TComponent>> componentPool = std::static_pointer_cast<Pool<TComponent>>(componentPools[componentId]);

    // If the entity id is greater than the current size of the component pool, then resize the pool
    if (entityId >= componentPool->GetSize()) {
        componentPool->Resize(numEntities);
    }

    // Create a new Component object of type T and forward the various parameters to the constructor
    TComponent newComponent(std::forward<TArgs>(args)...);

    // Add the new Component to the Component pool list, using the entity id as index
    componentPool->Set(entityId, newComponent);

    // Finally, change the component signature of the entity and set the component id on the bitset to 1
    entityComponentSignatures[entityId].set(componentId);

    Logger::Log("Component id: " + std::to_string(componentId) + " was added to entity id " + std::to_string(entityId));
}

template <typename TComponent> 
void Registry::RemoveComponent(Entity entity) {
    const auto componentId = Component<TComponent>::GetId();
    const auto entityId = entity.GetId();

    entityComponentSignatures[entityId].set(componentId, false);

    Logger::Log("Component id: " + std::to_string(componentId) + " was removed from entity id " + std::to_string(entityId));
}

template <typename TComponent>
bool Registry::HasComponent(Entity entity) const {
    const auto componentId = Component<TComponent>::GetId();
    const auto entityId = entity.GetId();

    return entityComponentSignatures[entityId].test(componentId);
}

template <typename TComponent> 
TComponent& Registry::GetComponent(Entity entity) const {
    const auto componentId = Component<TComponent>::GetId();
    const auto entityId = entity.GetId();
    auto componentPool = std::static_pointer_cast<Pool<TComponent>>(componentPools[componentId]);
    return componentPool->Get(entityId);
}

template <typename TSystem, typename ...TArgs> 
void Registry::AddSystem(TArgs&& ...args) {
    std::shared_ptr<TSystem> newSystem = std::make_shared<TSystem>(std::forward<TArgs>(args)...);
    systems.insert(std::make_pair(std::type_index(typeid(TSystem)), newSystem));
}

template <typename TSystem> 
void Registry::RemoveSystem() {
    auto system = systems.find(std::type_index(typeid(TSystem)));
    systems.erase(system);
}

template <typename TSystem> 
bool Registry::HasSystem() const {
    return systems.find(std::type_index(typeid(TSystem))) != systems.end();
}

template <typename TSystem> 
TSystem& Registry::GetSystem() const {
    auto system = systems.find(std::type_index(typeid(TSystem)));
    return *(std::static_pointer_cast<TSystem>(system->second));
}

template <typename TComponent, typename ...TArgs> 
void Entity::AddComponent(TArgs&& ...args) {
    registry->AddComponent<TComponent>(*this, std::forward<TArgs>(args)...);
}

template <typename TComponent> 
void Entity::RemoveComponent() {
    registry->RemoveComponent<TComponent>(*this);
}

template <typename TComponent> 
bool Entity::HasComponent() const {
    return registry->HasComponent<TComponent>(*this);
}

template <typename TComponent> 
TComponent& Entity::GetComponent() const {
    return registry->GetComponent<TComponent>(*this);
}

#endif