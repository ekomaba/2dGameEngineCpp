#ifndef DAMAGESYSTEM_H
#define DAMAGESYSTEM_H

#include "../ECS/ECS.h"
#include "../Components/BoxColliderComponent.h"
#include "../Components/ProjectileComponent.h"
#include "../Components/HealthComponent.h"
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
        // testing bullet spawning
        Entity a = event.a;
        Entity b = event.b;
        
        Logger::Log("The Damage system received an event collision between entities: " + 
            std::to_string(a.GetId()) + " and " + std::to_string(b.GetId()));

    
        if (a.BelongsToGroup("projectiles") && b.HasTag("player")) {
            OnProjectileHitsPlayer(a, b); // a projectile, b player
        }

        if (b.BelongsToGroup("projectiles") && a.HasTag("player")) {
            OnProjectileHitsPlayer(b, a); // b projectile, a player
        }

        if (a.BelongsToGroup("projectiles") && b.BelongsToGroup("enemies")) {
            OnProjectileHitsEnemy(a, b);
        }

        if (b.BelongsToGroup("projectiles") && a.BelongsToGroup("enemies")) {
            OnProjectileHitsEnemy(b, a);
        }
    }

    void OnProjectileHitsPlayer(Entity projectile, Entity player) {
        auto projectileComponent = projectile.GetComponent<ProjectileComponent>();

        if (!projectileComponent.isFriendly) {
            auto& health = player.GetComponent<HealthComponent>();
            health.healthPercentage -= projectileComponent.hitPercentDamage;

            if (health.healthPercentage <= 0) {
                player.Kill();
            }

            projectile.Kill();
        }
    }

    void OnProjectileHitsEnemy(Entity projectile, Entity enemy) {
        const auto projectileComponent = projectile.GetComponent<ProjectileComponent>();

        // Only damage the enemy if projectile is friendly
        if (projectileComponent.isFriendly) {
            auto& health = enemy.GetComponent<HealthComponent>();

            // Subtract from enemy health
            health.healthPercentage -= projectileComponent.hitPercentDamage;

            // Kills the enemy if health reaches zero
            if (health.healthPercentage <= 0) {
                enemy.Kill();
            }

            // Destroy projectile
            projectile.Kill();
        }
    }

    void Update() {}
};

#endif