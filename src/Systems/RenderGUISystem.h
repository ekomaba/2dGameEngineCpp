#ifndef RENDERGUISYSTEM_H
#define RENDERGUISYSTEM_H

#include "../ECS/ECS.h"
#include <imgui/imgui.h>
#include <imgui/imgui_sdl.h>
#include "../Components/TransformComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Components/BoxColliderComponent.h"
#include "../Components/ProjectileEmitterComponent.h"
#include "../Components/HealthComponent.h"

class RenderGUISystem : public System {
public:
    RenderGUISystem() = default;

    void Update(const std::unique_ptr<Registry>& registry) {
        // TODO: draw all the ImGui objects in the screen
        ImGui::NewFrame();

        if (ImGui::Begin("Spawn Enemies")) {
            // input for the enemy X position and Y position
            static int enemyXPos = 0;
            static int enemyYPos = 0;
            ImGui::InputInt("x position", &enemyXPos);
            ImGui::InputInt("y position", &enemyYPos);

            if (ImGui::Button("Create New Enemy")) {
                // button action
                Entity enemy = registry->CreateEntity();
                enemy.Group("enemies");
                enemy.AddComponent<TransformComponent>(glm::vec2(enemyXPos, enemyYPos), glm::vec2(1.0, 1.0), 0.0);
                enemy.AddComponent<RigidBodyComponent>(glm::vec2(0.0, 0.0));
                enemy.AddComponent<SpriteComponent>("tank-image", 32, 32, 2);
                enemy.AddComponent<BoxColliderComponent>(32, 32);
                enemy.AddComponent<ProjectileEmitterComponent>(glm::vec2(-100,0), 900, 1200, 10, false);
                enemy.AddComponent<HealthComponent>(50);
            }
        }

        ImGui::End();

        ImGui::Render();
        ImGuiSDL::Render(ImGui::GetDrawData());
    }
};
#endif