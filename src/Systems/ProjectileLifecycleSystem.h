#ifndef PROJECTILELIFECYCLESYSTEM_H
#define PROJECTILELIFECYCLESYSTEM_H

#include "../ECS/ECS.h"
#include "../Components/ProjectileComponent.h"
#include "../Logger/Logger.h"

class ProjectileLifecycleSystem : public System {
public:
    ProjectileLifecycleSystem() {
        RequireComponent<ProjectileComponent>();
    }

    void Update() {
        for (auto entity : GetSystemEntities()) {
            auto projectile = entity.GetComponent<ProjectileComponent>();

            // kill projectile after they reach their duration limit
            if (SDL_GetTicks() - projectile.startTime > projectile.duration) {
                entity.Kill();
            }
        }
    }
};

#endif