#ifndef DAMAGESYSTEM_H
#define DAMAGESYSTEM_H

#include "../ECS/ECS.h"
#include "../Components/BoxColliderComponent.h"
#include "../EventBus/EventBus.h"
#include "../Event/CollisionEvent.h"
#include "../Logger/Logger.h"

class DamageSystem : public System {
public:
    DamageSystem()  {
        RequireComponent<BoxColliderComponent>();
    }

    void SubscribeToEvents(std::unique_ptr<EventBus>& eventBus) {
        eventBus->SubscribeToEvent<CollisionEvent>(this, &DamageSystem::onCollision);
    }

    void onCollision(CollisionEvent& event) {
        Logger::Log("The Damage system received an event collision between entities: " + 
                    std::to_string(event.a.GetId()) + " and " + std::to_string(event.b.GetId()));

        // testing bullet spawning
        //event.a.Kill();
        //event.b.Kill();
    }

    void Update() {

    }
};

#endif