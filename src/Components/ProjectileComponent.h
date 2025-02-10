#ifndef PROJECTILECOMPONENT_H
#define PROJECTILECOMPONENT_H

#include <SDL2/SDL.h>

struct ProjectileComponent {
    bool isFriendly;
    int hitPercentDamage;
    Uint32 duration;
    Uint32 startTime;

    ProjectileComponent(bool isFriendly = false,
                        int hitPercentDamage = 0,
                        Uint32 duration = 0) 
    : isFriendly(isFriendly),
      hitPercentDamage(hitPercentDamage),
      duration(duration),
      startTime(SDL_GetTicks()) {}
};

#endif