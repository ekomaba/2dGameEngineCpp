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

    void Update(const std::unique_ptr<Registry>& registry, const SDL_Rect& camera) {
        // TODO: draw all the ImGui objects in the screen
        ImGui::NewFrame();

        if (ImGui::Begin("Spawn Enemies")) {
            // input for the enemy X position and Y position
            static int posX = 0;
            static int posY = 0;
            static int scaleX = 1;
            static int scaleY = 1;
            static int velX = 0;
            static int velY = 0;
            static int health = 100;
            static float rotation = 0.0;
            static float projAngle = 0.0;
            static float projSpeed = 100.0;
            static int projRepeat = 10;
            static int projDuration = 10;
            const char* sprites[] = {"tank-image", "truck-image"};
            static int selectedSpriteIndex = 0;

            // section to input enemy sprite texture id
            if (ImGui::CollapsingHeader("Sprite", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::Combo("texture id", &selectedSpriteIndex, sprites, IM_ARRAYSIZE(sprites));
            }

            ImGui::Spacing();

            // section to input enemy transform values
            if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::InputInt("position x", &posX);
                ImGui::InputInt("position y", &posY);
                ImGui::SliderInt("scale x", &scaleX, 1, 10);
                ImGui::SliderInt("scale Y", &scaleY, 1, 10);
                ImGui::SliderAngle("rotation (deg)", &rotation, 0, 360);
            }

            ImGui::Spacing();

            // section to input enemy rigid body values
            if (ImGui::CollapsingHeader("Rigid Body", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::InputInt("velocity x", &velX);
                ImGui::InputInt("velocity y", &velY);
            }

            ImGui::Spacing();

            // section to input enemy projectile emitter values
            if (ImGui::CollapsingHeader("Projectile emitter", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::SliderAngle("angle (deg)", &projAngle, 0, 360);
                ImGui::SliderFloat("speed (px/sec)", &projSpeed, 10, 500);
                ImGui::InputInt("repeat (sec)", &projRepeat);
                ImGui::InputInt("duration (sec)", &projDuration);
            }

            ImGui::Spacing();

            // section to input enemy health values
            if (ImGui::CollapsingHeader("Health", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::SliderInt("%", &health, 0, 100);
            }

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();

            if (ImGui::Button("Create New Enemy")) {
                // button action
                Entity enemy = registry->CreateEntity();
                enemy.Group("enemies");
                enemy.AddComponent<TransformComponent>(glm::vec2(posX, posY), glm::vec2(scaleX, scaleY), glm::degrees(rotation));
                enemy.AddComponent<RigidBodyComponent>(glm::vec2(velX, velY));
                enemy.AddComponent<SpriteComponent>(sprites[selectedSpriteIndex], 32, 32, 2);
                enemy.AddComponent<BoxColliderComponent>(32, 32, glm::vec2(5,5));
                double projVelX = cos(projAngle) * projSpeed;
                double projVelY = sin(projAngle) * projSpeed;
                enemy.AddComponent<ProjectileEmitterComponent>(glm::vec2(projVelX, projVelY), projRepeat * 1000, projDuration * 1000, 10, false);
                enemy.AddComponent<HealthComponent>(health);

                // reset all input values after we create a new enemy
                posX = posY = rotation = projAngle = 0;
                scaleX = scaleY = 1;
                projRepeat = projDuration = 10;
                projSpeed = 100;
                health = 100;
            }
        }

        ImGui::End();

        // display small overlay window to show the map postiion using the mouse
        ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoNav;
        ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Always, ImVec2(0, 0));
        ImGui::SetNextWindowBgAlpha(0.9f);
        if (ImGui::Begin("Map coordinates", NULL, windowFlags)) {
            ImGui::Text(
                "Map coordinates (x=%.1f, y=%.1f)",
                ImGui::GetIO().MousePos.x + camera.x,
                ImGui::GetIO().MousePos.y + camera.y
            );
        }

        ImGui::End();
        ImGui::Render();
        ImGuiSDL::Render(ImGui::GetDrawData());
    }
};
#endif