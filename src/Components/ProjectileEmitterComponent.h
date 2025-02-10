#ifndef PROJECTILEEMITTERCOMPONENT_H
#define PROJECTILEEMITTERCOMPONENT_H

#include <SDL2/SDL.h>
#include <glm/glm.hpp>

struct ProjectileEmitterComponent {

    glm::vec2 projectileVelocity;
    Uint32 repeatFrequency;
    Uint32 projectileDuration;
    int hitPercentDamage;
    bool isFriendly;
    Uint32 lastEmissionTime;
    
    ProjectileEmitterComponent(glm::vec2 projectileVelocity = glm::vec2(0), 
                               Uint32 repeatFrequency = 0, 
                               Uint32 projectileDuration = 3000, 
                               int hitPercentDamage = 10, 
                               bool isFriendly = false) 
                               
        : projectileVelocity(projectileVelocity),
        repeatFrequency(repeatFrequency),
        projectileDuration(projectileDuration),
        hitPercentDamage(hitPercentDamage),
        isFriendly(isFriendly),
        lastEmissionTime(SDL_GetTicks()) {}

};
#endif