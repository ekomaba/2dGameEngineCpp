#ifndef PROJECTILEEMITSYSTEM
#define PROJECTILEEMITSYSTEM

#include "../ECS/ECS.h"
#include "../Components/TransformComponent.h"
#include "../Components/ProjectileEmitterComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Components/ProjectileComponent.h"
#include <SDL2/SDL.h>

class ProjectileEmitSystem : public System {
public:
    ProjectileEmitSystem() {
        RequireComponent<TransformComponent>();
        RequireComponent<ProjectileEmitterComponent>();
    }

    void SubscribeToEvents(std::unique_ptr<EventBus>& eventBus) {
        eventBus->SubscribeToEvent<KeyPressedEvent>(this, &ProjectileEmitSystem::OnKeyPressed);
    }

    void OnKeyPressed(KeyPressedEvent& event) {
        if (event.symbol == SDLK_SPACE) {
            for (auto entity : GetSystemEntities()) {
                // if the entity has CameraFollowComponent its the player
                if (entity.HasComponent<CameraFollowComponent>()) {
                    const auto projectileEmitter = entity.GetComponent<ProjectileEmitterComponent>();
                    const auto transform = entity.GetComponent<TransformComponent>();
                    const auto rigidbody = entity.GetComponent<RigidBodyComponent>();

                    // if parent entity has sprite, start the projectile position in the middle
                    glm::vec2 projectilePosition = transform.position;
                    if (entity.HasComponent<SpriteComponent>()) {
                        auto sprite = entity.GetComponent<SpriteComponent>();
                        projectilePosition.x += (transform.scale.x * sprite.width / 2);
                        projectilePosition.y += (transform.scale.y * sprite.height / 2);
                    }

                    // if parent entity direction is controlled by the keyboard keys, modify the position
                    glm::vec2 projectileVelocity = projectileEmitter.projectileVelocity;
                    int directionX = 0;
                    int directionY = 0;
                    // velocity of the chopper
                    if (rigidbody.velocity.x > 0) directionX = +1;
                    if (rigidbody.velocity.x < 0) directionX = -1;
                    if (rigidbody.velocity.y > 0) directionY = +1;
                    if (rigidbody.velocity.y < 0) directionY = -1;
                    projectileVelocity.x = projectileEmitter.projectileVelocity.x * directionX;
                    projectileVelocity.y = projectileEmitter.projectileVelocity.y * directionY;

                    // Create new projectile entity and add it to the world
                    Entity projectile = entity.registry->CreateEntity();
                    projectile.AddComponent<TransformComponent>(projectilePosition, glm::vec2(1.0, 1.0), 0.0);
                    projectile.AddComponent<RigidBodyComponent>(projectileVelocity);
                    projectile.AddComponent<SpriteComponent>("bullet-image", 4, 4, 4);
                    projectile.AddComponent<BoxColliderComponent>(4, 4);
                    projectile.AddComponent<ProjectileComponent>(projectileEmitter.isFriendly,
                                                                 projectileEmitter.hitPercentDamage,
                                                                 projectileEmitter.projectileDuration);

                }
            }
        }
    }

    void Update(std::unique_ptr<Registry>& registry) {
        for (auto entity : GetSystemEntities()) {
            const auto transform = entity.GetComponent<TransformComponent>();
            auto& projectileEmitter = entity.GetComponent<ProjectileEmitterComponent>();

            // if emission frequency is zero bypass re-emission logic
            if (projectileEmitter.repeatFrequency == 0) {
                continue;
            }

            if ((SDL_GetTicks() - projectileEmitter.lastEmissionTime) > projectileEmitter.repeatFrequency) {

                glm::vec2 projectilePosition = transform.position;

                // getting the middle of the sprite, the original point of projectile spawn
                if (entity.HasComponent<SpriteComponent>()) {
                    const auto sprite = entity.GetComponent<SpriteComponent>();
                    projectilePosition.x += (transform.scale.x * sprite.width/2);
                    projectilePosition.y += (transform.scale.y * sprite.height/2);
                }
                
                Entity projectile = registry->CreateEntity();
                projectile.AddComponent<TransformComponent>(projectilePosition, glm::vec2(1.0,1.0), 0.0);
                projectile.AddComponent<RigidBodyComponent>(projectileEmitter.projectileVelocity);
                projectile.AddComponent<SpriteComponent>("bullet-image", 4, 4, 4);
                projectile.AddComponent<BoxColliderComponent>(4, 4);
                projectile.AddComponent<ProjectileComponent>(projectileEmitter.isFriendly, 
                                                             projectileEmitter.hitPercentDamage,
                                                             projectileEmitter.projectileDuration);

                projectileEmitter.lastEmissionTime = SDL_GetTicks();
            }
        }
    }
};
#endif