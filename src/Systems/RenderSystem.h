#ifndef RENDERSYSTEM_H
#define RENDERSYSTEM_H

#include "../ECS/ECS.h"
#include "../Components/SpriteComponent.h"
#include "../Components/TransformComponent.h"
#include "../AssetStore/AssetStore.h"
#include <SDL2/SDL.h>

class RenderSystem : public System {
public:    
    RenderSystem() {
        RequireComponent<SpriteComponent>();
        RequireComponent<TransformComponent>();
    }

    void Update(SDL_Renderer* renderer, std::unique_ptr<AssetStore>& assetStore) {
        for (auto entity : GetSystemEntities()) {
            const auto transform = entity.GetComponent<TransformComponent>();
            const auto sprite = entity.GetComponent<SpriteComponent>();

            // Define the portion of the sprite texture to render
            SDL_Rect srcRect = sprite.srcRect;

            // Define the position and size of the sprite on the screen
            SDL_Rect dstRect = {
                static_cast<int>(transform.position.x),
                static_cast<int>(transform.position.y),
                static_cast<int>(sprite.width * transform.scale.x),
                static_cast<int>(sprite.height * transform.scale.y)
            };

            // Renders the texture with rotation, scaling, and flipping options
            SDL_RenderCopyEx(
                renderer, 
                assetStore->GetTexture(sprite.assetId),
                &srcRect,
                &dstRect,
                transform.rotation,
                NULL,
                SDL_FLIP_NONE);
        }
    }
};

#endif