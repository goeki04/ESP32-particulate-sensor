#include "pch.h"
#include "panels.h"
#include "GuiRenderer.h"
#include "components.h"
#include "Registry.h"
void Gui::Panels::drawDeviceHierarchy(GuiRenderer& guiRenderer)
{
    ImVec2 windowSize = ImVec2(guiRenderer.m_WidgetWidth, guiRenderer.m_WindowHeight * 0.55f);
    ImVec2 newPos = guiRenderer.getNewWindowPos(Margin(guiRenderer.m_MarginDefault, 0, guiRenderer.m_MarginDefault, guiRenderer.m_MarginDefault), windowSize, Alignment::TopLeft);
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 0.0f);
    ImGui::SetNextWindowPos(newPos, ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(windowSize, ImGuiCond_FirstUseEver);
    ImGui::Begin("Hierarchy", 0, guiRenderer.m_WindowFlags);
    ImGui::BeginChild("HierarchyList", ImVec2(0.0f, 0.0f), true, ImGuiWindowFlags_NoScrollbar);
    auto& transformPool = guiRenderer.m_Registry->getPool<ECS::component::Transform>();
    const auto& entityIDs = transformPool.getEntities();
    ImGuiListClipper clipper;
    clipper.Begin((int)entityIDs.size());
    while (clipper.Step())
    {
        for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
        {
            const ECS::Entity& e = entityIDs[i];
            ECS::EntityHandle handle = { e, guiRenderer.m_Registry };
            bool isSelected = (guiRenderer.m_CurrentSelectedID == e);
            if (isSelected) {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.5f, 0.0f, 1.0f));
            }
            std::string label;
            if (handle.has<ECS::component::Tag>()) {
                label = handle.get<ECS::component::Tag>().name;
            }
            else {
                label = "Unnamed";
            }
            if (ImGui::Selectable(label.c_str(), isSelected)) {
                guiRenderer.m_CurrentSelectedID = e;
                auto& selectedPool = guiRenderer.m_Registry->getPool<ECS::component::Selected>();
                std::vector<ECS::Entity> toDeselect = selectedPool.getEntities();
                for (ECS::Entity oldE : toDeselect) {
                    selectedPool.removeEntity(oldE);
                };
                handle.add<ECS::component::Selected>({});
            }
            if (isSelected) {
                ImGui::PopStyleColor();
            }
            if (ImGui::BeginPopupContextItem())
            {
                guiRenderer.m_CurrentSelectedID = e;

                ImGui::Text("Context");
                ImGui::Separator();

                if (ImGui::MenuItem("Delete")) {

                    guiRenderer.m_Registry->destroyEntity(e);
                    if (guiRenderer.m_CurrentSelectedID == e) {
                        guiRenderer.m_CurrentSelectedID = -1;
                    }
                }
                ImGui::EndPopup();
            }
        }
    }

    ImGui::EndChild();
    ImGui::PopStyleVar();
    ImGui::End();
}