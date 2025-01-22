#ifndef MOVEMENTSYSTEM_H
#define MOVEMENTSYSTEM_H

#include "../ECS/ECS.h"
#include "../Components/TransformComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Logger/Logger.h"

class MovementSystem : public System {

public:
    MovementSystem() {
        RequireComponent<TransformComponent>();
        RequireComponent<RigidBodyComponent>();
    }

    void Update(double deltatime) {
        for (auto entity : GetSystemEntities()) {
            auto& transform = entity.GetComponent<TransformComponent>();
            const auto rigidbody = entity.GetComponent<RigidBodyComponent>();

            transform.position.x += rigidbody.velocity.x * deltatime;
            transform.position.y += rigidbody.velocity.y * deltatime;

            Logger::Log("Entity id = " + std::to_string(
                entity.GetId()) + " position is now (" + 
                std::to_string(transform.position.x) + ", " + 
                std::to_string(transform.position.y) + ")");
        }
    }
};

#endif