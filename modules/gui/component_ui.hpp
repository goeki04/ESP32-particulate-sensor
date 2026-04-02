#pragma once

#include "a_registry.hpp"
#include "imgui.h"
#include "generated_components.hpp"
namespace Andromeda::Gui::Component
{
    /*
    template<typename T>
    inline void drawComponentUI(ECS::EntityHandle handle)
    {
    }
    template<>
    inline void drawComponentUI<ECS::Component::Transform>(ECS::EntityHandle handle)
    {
            handle.get<ECS::Component::Transform>();
            ImGui::PushID("Transform");
            ImGui::Text("Position");
            ImGui::SameLine(100);
            ImGui::DragFloat3("##pos", &transform.position.x, 0.1f);

            ImGui::Text("Rotation");
            ImGui::SameLine(100);
            ImGui::DragFloat3("##rot", &transform.rotation.x, 0.1f);

            ImGui::Text("Scale");
            ImGui::SameLine(100);
            ImGui::DragFloat3("##scale", &transform.scale.x, 0.1f);

            ImGui::PopID();
            ImGui::Spacing();
    }

    inline void renderEntityComponentUI(ECS::EntityHandle handle)
    {
        std::apply([&]<typename... T>(T&&... args)
        {

            if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
            {
                (drawComponentUI<T>(handle), ...);
            }
        },ECS::Component::ComponentDirectory{});
    }
    */
}
