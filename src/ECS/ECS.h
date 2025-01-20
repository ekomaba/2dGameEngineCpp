#ifndef ECS_H
#define ECS_H
#include <bitset>
#include <vector>
#include <algorithm>

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
    inline int GetId() const { return id; }
    bool operator ==(const Entity& other) const { return id == other.id; }
    bool operator !=(const Entity& other) const { return id != other.id; }
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

class Registry {
    //TODO
};

// Template function to require a component in a system by setting the appropriate bit in the signature
template<typename TComponent> 
void System::RequireComponent() {
    const auto componentId = Component<TComponent>::GetId(); // Generates a unique ID for each component type
    componentSignature.set(componentId); // Marks the component as required by setting the corresponding bit to true
}
#endif