#pragma once
#include "a_registry.hpp"
#include "imgui.h"
#include "generated_components.hpp"
#include <any>
#include "resource_manager.h"
#include "a_event_manager.hpp"
#include "a_subsystem_manager.hpp"
#include "a_shader_generated.hpp"
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

    template<>
    inline void drawComponentUI<ECS::Component::Tag>(ECS::EntityHandle handle, std::any& undoState){
        auto& tag = handle.get<ECS::Component::Tag>();

        ImGui::PushID("TagComponent");
        ImGui::Text("Name");
        ImGui::SameLine(100);
        char buffer[256];
        std::memset(buffer, 0, sizeof(buffer));
        std::strncpy(buffer, tag.name.c_str(), sizeof(buffer) - 1);

        if (ImGui::InputText("##tag_name", buffer, sizeof(buffer)))
        {
            tag.name = std::string(buffer);
        }

        ImGui::PopID();
        ImGui::Spacing();
    }

    template<>
    inline void drawComponentUI<ECS::Component::Material>(ECS::EntityHandle handle, std::any& undoState) {
        ResourceManager* rm = SystemManager::getInstance().getSubsystem<ResourceManager>();
        auto& matComp = handle.get<ECS::Component::Material>();

        auto material = rm->getMaterial(matComp.materialName);
        if (!material || !material->m_UboStructure.has_value()) {
            ImGui::TextDisabled("Keine editierbaren UBO-Daten für dieses Material.");
            return;
        }

        ImGui::PushID("MaterialComponent");

        bool valueChanged = false;

        auto imGuiVisitor = [&](const char* name, auto& value, ShaderDataType type, float min = 0.0f, float max = 0.0f) {
            std::string labelId = std::string("##ubo_") + name;

            if (type == ShaderDataType::Mat3 || type == ShaderDataType::Mat4) return;

            ImGui::Text("%s", name);
            ImGui::SameLine(100);

            using T = std::decay_t<decltype(value)>;

            bool useSlider = (min != max);

            if constexpr (std::is_same_v<T, float>) {
                if (useSlider) {
                    if (ImGui::SliderFloat(labelId.c_str(), &value, min, max, "%.3f", ImGuiSliderFlags_AlwaysClamp)) valueChanged = true;
                }
                else {
                    if (ImGui::DragFloat(labelId.c_str(), &value, 0.05f)) valueChanged = true;
                }
            }
            else if constexpr (std::is_same_v<T, vec2>) {
                if (useSlider) {
                    if (ImGui::SliderFloat2(labelId.c_str(), &value.x, min, max, "%.3f", ImGuiSliderFlags_AlwaysClamp)) valueChanged = true;
                }
                else {
                    if (ImGui::DragFloat2(labelId.c_str(), &value.x, 0.05f)) valueChanged = true;
                }
            }
            else if constexpr (std::is_same_v<T, vec3>) {
                if (useSlider) {
                    if (ImGui::SliderFloat3(labelId.c_str(), &value.x, min, max, "%.3f", ImGuiSliderFlags_AlwaysClamp)) valueChanged = true;
                }
                else {
                    if (ImGui::DragFloat3(labelId.c_str(), &value.x, 0.05f)) valueChanged = true;
                }
            }
            else if constexpr (std::is_same_v<T, vec4>) {
                if (useSlider) {
                    if (ImGui::SliderFloat4(labelId.c_str(), &value.x, min, max, "%.3f", ImGuiSliderFlags_AlwaysClamp)) valueChanged = true;
                }
                else {
                    if (ImGui::DragFloat4(labelId.c_str(), &value.x, 0.05f)) valueChanged = true;
                }
            }
            else if constexpr (std::is_same_v<T, i32>) {
                if (useSlider) {
                    if (ImGui::SliderInt(labelId.c_str(), &value, static_cast<int>(min), static_cast<int>(max), "%d", ImGuiSliderFlags_AlwaysClamp)) valueChanged = true;
                }
                else {
                    if (ImGui::InputInt(labelId.c_str(), &value)) valueChanged = true;
                }
            }

            if (ImGui::IsItemActivated()) {
                undoState = matComp;
            }
            };

        if (matComp.materialName == "PBRMaterial") {
            auto& pbrData = std::any_cast<Generated::pbrMaterial&>(material->m_UboStructure);
            pbrData.reflect(imGuiVisitor);
            if (valueChanged) material->updateUBOData(pbrData);
        }

        ImGui::PopID();
        ImGui::Spacing();
    }

    inline void renderEntityComponentUI(const ECS::EntityHandle handle, ResourceManager* rm, std::any& undoState)
    {
        using AllowedComponents = std::tuple<
            ECS::Component::Tag,        
            ECS::Component::Transform,  
            ECS::Component::Material    
        >;

        std::apply([&]<typename... T>(T&&... args)
        {
            ([&]() {
                if (handle.registry->hasComponent<T>(handle.id))
                {
                    std::string componentName = typeid(T).name();
                    size_t lastScope = componentName.find_last_of("::");
                    if (lastScope != std::string::npos) {
                        componentName = componentName.substr(lastScope + 1);
                    }

                    if (ImGui::CollapsingHeader(componentName.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
                    {
                        drawComponentUI<T>(handle, undoState);
                    }
                }
                }(), ...);
        }, AllowedComponents{});
    }
}
