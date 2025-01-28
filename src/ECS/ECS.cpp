#include "ECS.h"

int IComponent::nextId = 0;

void System::AddEntity(Entity entity) {
    entities.push_back(entity);
}

void System::RemoveEntity(Entity entity) {
    entities.erase(std::remove_if(
        entities.begin(), 
        entities.end(),
        [entity](const Entity& other) { return entity == other; }), 
        entities.end());
}

std::vector<Entity> System::GetSystemEntities() const {
    return entities;
}

const Signature& System::GetComponentSignature() const {
    return componentSignature;
}

Registry::Registry() {
    Logger::Log("Registry Constructor Called!");
}

Registry::~Registry() {
    Logger::Log("Registry Destructor Called!");
}

Entity Registry::CreateEntity() {
    std::size_t entityId;

    if (freeIds.empty()) {
        // If there are no free ids waiting to be reused
        entityId = numEntities++;

        // Resize if needs it
        if (entityId >= entityComponentSignatures.size()) { 
            entityComponentSignatures.resize(entityId + 1);
        }
    } else {
        // Reuse an id from the list of removed entities
        entityId = freeIds.front();
        freeIds.pop_front();
    }

    Entity entity(entityId);
    entity.registry = this;    
    entitiesToBeAdded.insert(entity);

    Logger::Log("Entity created with id = " + std::to_string(entityId));

    return entity;
}

void Entity::Kill() {
    registry->KillEntity(*this);
}

void Registry::KillEntity(Entity entity) {
    entitiesToBeKilled.insert(entity);
}

void Registry::AddEntityToSystems(Entity entity) {
    const auto entityId = entity.GetId();
    const auto& entityComponentSignature = entityComponentSignatures[entityId];

    for (auto& system : systems) {
        const auto& systemComponentSignature = system.second->GetComponentSignature();

        bool isInterested =  (entityComponentSignature & systemComponentSignature) == systemComponentSignature;

        if (isInterested) {
            system.second->AddEntity(entity);
        }
    }
}

void Registry::RemoveEntityFromSystems(Entity entity) {
    for (auto system : systems) {
        system.second->RemoveEntity(entity);
    }
}

void Registry::Update() {
    // Process the entities that are waiting to be created to the active Systems
    for (auto entity : entitiesToBeAdded) {
        AddEntityToSystems(entity);
    }
    entitiesToBeAdded.clear();

    // Process the entities that are waiting to be killed from the active Systems
    for (auto entity : entitiesToBeKilled) {
        RemoveEntityFromSystems(entity);
        entityComponentSignatures[entity.GetId()].reset();
        freeIds.push_back(entity.GetId());
    }

    entitiesToBeKilled.clear();
}