#include "a_HierarchyPanel.hpp"
#include <imgui.h>
#include "a_components.hpp"
#include "a_SelectionContext.hpp"
#include "a_EditorContext.hpp"
#include "a_registry.hpp"
#include "a_event_manager.hpp"
namespace Andromeda::Gui{
    std::string HierarchyPanel::getEntityName(ECS::EntityHandle& handle)
    {
        if (handle.has<ECS::Component::Tag>()) {
            return handle.get<ECS::Component::Tag>().name;
        }
        return "Unnamed";
    }
    void HierarchyPanel::selectEntity(EditorContext& ctx,const ECS::Entity entity)
    {
        if (entity != ctx.selection->getSelectedEntity())
        {
            ctx.selection->setSelectedEntity(entity);
            EventManager::getInstance().Dispatch(EventType::OnSceneObjectSelected, SceneObjectSelected(static_cast<u32>(entity)));
        }
    }

    void HierarchyPanel::drawEntityNode(EditorContext& ctx, const ECS::Entity e)
    {
        const ECS::Entity currentSelected = ctx.selection->getSelectedEntity();
        ImGui::PushID(static_cast<i32>(e));
        ECS::EntityHandle handle = { e, ctx.registry };
        const bool isSelected = (currentSelected == e);
        const std::string label = getEntityName(handle);

        if (isSelected) {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.5f, 0.0f, 1.0f));
        }
        if (ImGui::Selectable(label.c_str(), isSelected)) {
            selectEntity(ctx, e);
        }
        if (isSelected) {
            ImGui::PopStyleColor();
        }
        if (ImGui::BeginPopupContextItem()) {
            selectEntity(ctx, e);
            ImGui::Text("Context");
            ImGui::Separator();
            if (ImGui::MenuItem("Delete")) {
                ctx.registry->destroyEntity(e);
                if (ctx.selection->getSelectedEntity() == e) {
                    ctx.selection->setSelectedEntity(ECS::INVALID_ENTITY_ID);
                    EventManager::getInstance().Dispatch(EventType::OnSceneObjectSelected, SceneObjectSelected(static_cast<u32>(ECS::INVALID_ENTITY_ID)));
                }
            }
            ImGui::EndPopup();
        }
        ImGui::PopID();
    }

    void HierarchyPanel::onGuiRender(EditorContext& ctx)
    {
        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 0.0f);
        if (ImGui::Begin(m_Name, &m_IsOpen)) {
            if (ImGui::BeginChild("HierarchyList", ImVec2(0.0f, 0.0f), true, ImGuiWindowFlags_NoScrollbar)) {
                const auto& transformPool = ctx.registry->getPool<ECS::Component::Transform>();
                const auto& entityIDs = transformPool.getEntities();
                ImGuiListClipper clipper;
                clipper.Begin(static_cast<int>(entityIDs.size()));

                while (clipper.Step()) {
                    for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++) {
                        drawEntityNode(ctx, entityIDs[i]);
                    }
                }
                ImGui::EndChild();
            }
        }
        ImGui::End();
        ImGui::PopStyleVar();
    }
}
