#ifndef ECS_H
#define ECS_H
#include <bitset>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <typeindex>
#include <set>
#include <memory>
#include <deque>
#include "../Logger/Logger.h"

#include <iostream>

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
    void Kill();

    // Manage entity tags and groups
    void Tag(const std::string& tag);
    bool HasTag(const std::string& tag) const;
    void Group(const std::string& group);
    bool BelongsToGroup(const std::string& group) const;

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
    virtual ~IPool() = default;
    virtual void RemoveEntityFromPool(int entityId) = 0;
};

// Pool 
template<typename T>
class Pool : public IPool {
private:
    std::vector<T> data;
    int size;

    // helper maps to keep track of entity ids per index, so the vector is always packed
    std::unordered_map<int, int> entityIdToIndex;
    std::unordered_map<int, int> indexToEntityId;

public:
    Pool(int capacity = 100) { 
        size = 0; 
        data.resize(capacity); 
    }

    virtual ~Pool() = default;

    bool IsEmpty() const { return size == 0; }
    int GetSize() const { return size; }
    void Resize(int n) { data.resize(n); }

    void Clear() { 
        data.clear(); 
        size = 0;
    }

    void Add(T object) { data.push_back(object); }

    void Set(int entityId, T object) { 
        // if we found this entityId in my pool...
        if (entityIdToIndex.find(entityId) != entityIdToIndex.end()) {
            // just need to replace this values
            int index = entityIdToIndex[entityId];
            data[index] = object;

        } else {
            // else we need to add a new object, keep track of the entity ids and their vector indexes
            int index = size;
            entityIdToIndex.emplace(entityId, index);
            indexToEntityId.emplace(index, entityId);
            
            if (static_cast<size_t>(index) >= data.capacity()) {
                // if necessary, we resize by always doubling the current capacity
                data.resize(size * 2);
            }

            data[index] = object;
            size++;
        } 
    }

    void Remove(int entityId) {
        // Get the index of the entity we want to remove
        int indexOfRemoved = entityIdToIndex[entityId];
    
        // Get the index of the last element in the array
        int indexOfLast = size - 1;
    
        // Move the last element to the position of the removed entity to keep the array packed
        data[indexOfRemoved] = data[indexOfLast];
    
        // Update mappings to reflect the change
        int entityIdOfLastElement = indexToEntityId[indexOfLast]; // Get the entity ID of the last element
        entityIdToIndex[entityIdOfLastElement] = indexOfRemoved;  // The entityId of the last element have the index from the removed entityId
        indexToEntityId[indexOfRemoved] = entityIdOfLastElement;  // The index of the entity removed will point to the last element
    
        // Remove the old references to the deleted entity
        entityIdToIndex.erase(entityId);
        indexToEntityId.erase(indexOfLast);
        
        // Remove size
        size --;
    }

    void RemoveEntityFromPool(int entityId)  override {
        if (entityIdToIndex.find(entityId) != entityIdToIndex.end()) {
            Remove(entityId);
        }
    }

    T& Get(int entityId) { 
        int index = entityIdToIndex[entityId];
        return static_cast<T&>(data[index]); 
    }

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
    std::unordered_map<std::string, Entity> entityPerTag;
    std::unordered_map<int, std::string> tagPerEntity;
    std::unordered_map<std::string, std::set<Entity>> entitiesPerGroup;
    std::unordered_map<int, std::string> groupPerEntity;
    std::deque<int> freeIds;

public:
    Registry();
    ~Registry();

    // Entity management
    Entity CreateEntity();

    void KillEntity(Entity entity);

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

    // Add and remove systems from the systems
    void AddEntityToSystems(Entity entity);
    void RemoveEntityFromSystems(Entity entity);

    // Tag Management
    void TagEntity(Entity entity, const std::string& tag);
    bool EntityHasTag(Entity entity, const std::string& tag) const;
    Entity GetEntityByTag(const std::string& tag) const;
    void RemoveEntityTag(Entity entity);

    // Group Management
    void GroupEntity(Entity entity, const std::string& group);
    bool EntityBelongsToGroup(Entity entity, const std::string& group) const;
    std::vector<Entity> GetEntitiesByGroup(const std::string& group) const;
    void RemoveEntityGroup(Entity entity);
    
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

    // Remove the compnent from the component list for that entity
    std::shared_ptr<Pool<TComponent>> componentPool = std::static_pointer_cast<Pool<TComponent>>(componentPools[componentId]);
    componentPool->Remove(entityId);

    // Set this component signature for that entity to false
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