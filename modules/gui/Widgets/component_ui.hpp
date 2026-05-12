#pragma once
#include "a_registry.hpp"
#include "imgui.h"
#include "generated_components.hpp"
#include <any>
#include "resource_manager.h"
#include "a_event_manager.hpp"
namespace Andromeda::Gui::Component
{
    template<typename T>
    inline void drawComponentUI(ECS::EntityHandle handle, std::any& undoState){
    }
    template<>
    inline void drawComponentUI<ECS::Component::Transform>(ECS::EntityHandle handle, std::any& undoState)
    {
            auto& transform = handle.get<ECS::Component::Transform>();
            auto& position = transform.position;
            auto& scale = transform.scale;
            ImGui::PushID("Transform");
            ImGui::Text("Position");
            ImGui::SameLine(100);
            ImGui::DragFloat3("##pos", &position.x, 0.1f);

            if (ImGui::IsItemActivated()) {
                undoState = transform;
            }

            if (ImGui::IsItemDeactivatedAfterEdit()) {
                EventManager::getInstance().Dispatch(EventType::OnPushUndoTransform,
                    PushUndoTransformEvent(handle.id, undoState));
            }
            ImGui::Text("Rotation");
            ImGui::SameLine(100);
            vec3 rotationRad = amath::eulerAngles(transform.rotation);
            vec3 rotationDeg = amath::degrees(rotationRad);
            if (ImGui::DragFloat3("##rot", &rotationDeg.x, 0.1f)) {
                transform.rotation = quat(amath::radians(rotationDeg));
            }

            if (ImGui::IsItemActivated()) undoState = transform;
            if (ImGui::IsItemDeactivatedAfterEdit()) {
                EventManager::getInstance().Dispatch(EventType::OnPushUndoTransform,
                    PushUndoTransformEvent(handle.id, undoState));
            }

            ImGui::Text("Scale");
            ImGui::SameLine(100);
            ImGui::DragFloat3("##scale", &scale.x, 0.1f);
            if (ImGui::IsItemActivated()) undoState = transform;
            if (ImGui::IsItemDeactivatedAfterEdit()) {
                EventManager::getInstance().Dispatch(EventType::OnPushUndoTransform,
                    PushUndoTransformEvent(handle.id, undoState));
            }
            ImGui::PopID();
            ImGui::Spacing();
    }

    inline void renderEntityComponentUI(const ECS::EntityHandle handle, ResourceManager* rm, std::any& undoState)
    {
        std::apply([&]<typename... T>(T&&... args)
        {
            if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
            {
                (drawComponentUI<T>(handle,undoState), ...);
            }
        },ECS::Component::ComponentDirectory{});
    }
}
