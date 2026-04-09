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

void Andromeda::Gui::Panels::selectEntity(GuiRenderer& guiRenderer, ECS::Entity entity)
{
    if (guiRenderer.m_CurrentSelectedID != ECS::INVALID_ENTITY_ID)
    {
        auto& selectedPool = guiRenderer.m_Registry->getPool<ECS::Component::Selected>();

        if (selectedPool.has(guiRenderer.m_CurrentSelectedID)) {
            selectedPool.removeEntity(guiRenderer.m_CurrentSelectedID);
        }
    }

    guiRenderer.m_CurrentSelectedID = entity;

    if (entity != ECS::INVALID_ENTITY_ID)
    {
        ECS::EntityHandle handle = { entity, guiRenderer.m_Registry };
        handle.add<ECS::Component::Selected>({});
    }
}

void Andromeda::Gui::Panels::drawEntityNode(GuiRenderer& guiRenderer, ECS::Entity e)
{
    ImGui::PushID(static_cast<i32>(e));
    ECS::EntityHandle handle = { e, guiRenderer.m_Registry };
    const bool isSelected = (guiRenderer.m_CurrentSelectedID == e);
    const std::string label = getEntityName(handle);

    if (isSelected) {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.5f, 0.0f, 1.0f));
    }

    if (ImGui::Selectable(label.c_str(), isSelected)) {
        selectEntity(guiRenderer, e);
    }

    if (isSelected) {
        ImGui::PopStyleColor();
    }

    if (ImGui::BeginPopupContextItem()) {
        guiRenderer.m_CurrentSelectedID = e;
        ImGui::Text("Context");
        ImGui::Separator();

        if (ImGui::MenuItem("Delete")) {
            guiRenderer.m_Registry->destroyEntity(e);
            if (guiRenderer.m_CurrentSelectedID == e) {
                guiRenderer.m_CurrentSelectedID = ECS::INVALID_ENTITY_ID;
            }
        }
        ImGui::EndPopup();
    }
    ImGui::PopID();
}

void Andromeda::Gui::Panels::drawDeviceHierarchy(GuiRenderer& guiRenderer)
{
    const auto windowSize = ImVec2(guiRenderer.m_WidgetWidth, static_cast<float>(guiRenderer.m_WindowHeight) * 0.55f);
    const auto newPos = guiRenderer.getNewWindowPos(Margin(guiRenderer.m_MarginDefault, 0, guiRenderer.m_MarginDefault, guiRenderer.m_MarginDefault), windowSize, Alignment::TopLeft);

    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 0.0f);
    ImGui::SetNextWindowPos(newPos, ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(windowSize, ImGuiCond_FirstUseEver);

    if (ImGui::Begin("Hierarchy", nullptr, guiRenderer.m_WindowFlags)) {
        if (ImGui::BeginChild("HierarchyList", ImVec2(0.0f, 0.0f), true, ImGuiWindowFlags_NoScrollbar)) {
            const auto& transformPool = guiRenderer.m_Registry->getPool<ECS::Component::Transform>();
            const auto& entityIDs = transformPool.getEntities();

            ImGuiListClipper clipper;
            clipper.Begin(static_cast<int>(entityIDs.size()));

            while (clipper.Step()) {
                for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++) {
                    drawEntityNode(guiRenderer, entityIDs[i]);
                }
            }
            ImGui::EndChild();
        }
    }
    ImGui::End();
    ImGui::PopStyleVar();
}