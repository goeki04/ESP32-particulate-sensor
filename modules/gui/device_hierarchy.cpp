#include "panels.h"
#include "gui_renderer.h"
#include "a_components.hpp"
#include "a_registry.hpp"

std::string Andromeda::Gui::Panels::getEntityName(ECS::EntityHandle handle)
{
    if (handle.has<ECS::Component::Tag>()) {
        return handle.get<ECS::Component::Tag>().name;
    }
    return "Unnamed";
}

void Andromeda::Gui::Panels::selectEntity(EditorContext& ctx, ECS::Entity entity)
{
    if (ctx.state.currentSelectedID != ECS::INVALID_ENTITY_ID)
    {
        auto& selectedPool = ctx.registry->getPool<ECS::Component::Selected>();

        if (selectedPool.has(ctx.state.currentSelectedID)) {
            selectedPool.removeEntity(ctx.state.currentSelectedID);
        }
    }

    ctx.state.currentSelectedID = entity;

    if (entity != ECS::INVALID_ENTITY_ID)
    {
        ECS::EntityHandle handle = { entity, ctx.registry };
        handle.add<ECS::Component::Selected>({});
    }
}

void Andromeda::Gui::Panels::drawEntityNode(EditorContext& ctx, ECS::Entity e)
{
    ImGui::PushID(static_cast<i32>(e));
    ECS::EntityHandle handle = { e, ctx.registry };
    const bool isSelected = (ctx.state.currentSelectedID == e);
    const std::string label = getEntityName(handle);

    if (isSelected) {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.5f, 0.0f, 1.0f));
    }

    if (ImGui::Selectable(label.c_str(), isSelected)) {
        selectEntity(ctx,e);
    }

    if (isSelected) {
        ImGui::PopStyleColor();
    }

    if (ImGui::BeginPopupContextItem()) {
        ctx.state.currentSelectedID = e;
        ImGui::Text("Context");
        ImGui::Separator();

        if (ImGui::MenuItem("Delete")) {
            ctx.registry->destroyEntity(e);
            if (ctx.state.currentSelectedID == e) {
                ctx.state.currentSelectedID = ECS::INVALID_ENTITY_ID;
            }
        }
        ImGui::EndPopup();
    }
    ImGui::PopID();
}

void Andromeda::Gui::Panels::drawDeviceHierarchy(EditorContext& ctx)
{
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 0.0f);

    if (ImGui::Begin("Hierarchy", nullptr)) {
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