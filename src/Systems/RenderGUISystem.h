#ifndef RENDERGUISYSTEM_H
#define RENDERGUISYSTEM_H

#include "../ECS/ECS.h"
#include <imgui/imgui.h>
#include <imgui/imgui_sdl.h>

class RenderGUISystem : public System {
public:
    RenderGUISystem() = default;

    void Update() {
        // TODO: draw all the ImGui objects in the screen
        ImGui::NewFrame();

        if (ImGui::Begin("Spawn Enemies")) {
            ImGui::Text("dear imgui says hello. (%s)", IMGUI_VERSION);
            ImGui::Text("Here is where we spawn new enemies...");
        }

        ImGui::End();

        ImGui::Render();
        ImGuiSDL::Render(ImGui::GetDrawData());
    }
};
#endif