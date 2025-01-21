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

Entity Registry::CreateEntity() {
    int entityId = numEntities++;
    Entity entity(entityId);
    entitiesToBeAdded.insert(entity); // Going to be add on next update cicle
    return entity;

    Logger::Log("Entity created with id = " + std::to_string(entityId));
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

void Registry::Update() {

}

