#ifndef RENDERCOLLIDERSYSTEM_H
#define RENDERCOLLIDERSYSTEM_H

#include "../ECS/ECS.h"
#include "../Components/BoxColliderComponent.h"
#include "../SDL2/SDL.h"

class RenderColliderSystem : public System {
public:
    RenderColliderSystem() {
        RequireComponent<TransformComponent>();
        RequireComponent<BoxColliderComponent>();
    }

    void Update(SDL_Renderer *renderer, SDL_Rect& camera) {
        for (auto entity : GetSystemEntities()) {
            const auto transform = entity.GetComponent<TransformComponent>();
            const auto boxcollider = entity.GetComponent<BoxColliderComponent>();
            
            SDL_Rect colliderRect = {
                static_cast<int>(transform.position.x + boxcollider.offset.x - camera.x),
                static_cast<int>(transform.position.y + boxcollider.offset.y - camera.y),
                static_cast<int>(boxcollider.width * transform.scale.x),
                static_cast<int>(boxcollider.height * transform.scale.y)
            };

            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
            SDL_RenderDrawRect(renderer, &colliderRect);
        }
    }
};

#endif