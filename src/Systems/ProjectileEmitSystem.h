#ifndef PROJECTILEEMITSYSTEM
#define PROJECTILEEMITSYSTEM

#include "../ECS/ECS.h"
#include "../Components/TransformComponent.h"
#include "../Components/ProjectileEmitterComponent.h"
#include "../Components/SpriteComponent.h"
#include <SDL2/SDL.h>

class ProjectileEmitSystem : public System {
public:
    ProjectileEmitSystem() {
        RequireComponent<TransformComponent>();
        RequireComponent<ProjectileEmitterComponent>();
    }

    void Update(std::unique_ptr<Registry>& registry) {
        for (auto entity : GetSystemEntities()) {
            const auto transform = entity.GetComponent<TransformComponent>();
            auto& projectileEmitter = entity.GetComponent<ProjectileEmitterComponent>();

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
                
                projectileEmitter.lastEmissionTime = SDL_GetTicks();
            }
        }
    }
};
#endif