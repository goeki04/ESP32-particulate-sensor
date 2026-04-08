#include "a_event_manager.hpp"
#include "panels.h"
#include "gui_renderer.h"
#include "component_ui.hpp"
#include <iostream>

namespace Andromeda::Gui::Panels
{
    void drawAddComponentButton(ECS::EntityHandle handle)
    {
        const ImVec2 buttonPos = ImGui::GetCursorScreenPos();
        const float buttonWidth = ImGui::GetContentRegionAvail().x;

        if (ImGui::Button("Add Component", ImVec2(buttonWidth, 0))) {
            ImGui::OpenPopup("ComponentSearchPopup");
        }

        const auto popupPos = ImVec2(buttonPos.x, buttonPos.y + ImGui::GetFrameHeightWithSpacing());

        ImGui::SetNextWindowPos(popupPos);
        ImGui::SetNextWindowSize(ImVec2(buttonWidth, 0));

        if (ImGui::BeginPopup("ComponentSearchPopup")) {
            static char searchBuffer[64] = "";
            if (ImGui::IsWindowAppearing()) {
                ImGui::SetKeyboardFocusHere();
                searchBuffer[0] = '\0';
            }

            ImGui::InputTextWithHint("##Filter", "Search...", searchBuffer, IM_ARRAYSIZE(searchBuffer));
            ImGui::Separator();

            ImGui::EndPopup();
        }
    }

    void drawDetails(const GuiRenderer& guiRenderer)
    {
        const float height = static_cast<float>(guiRenderer.m_WindowHeight) - guiRenderer.m_MenuBarHeight - guiRenderer.
            m_MarginDefault * 2.0f;
        const auto windowSize = ImVec2(guiRenderer.m_WidgetWidth, height);
        const ImVec2 newPos = guiRenderer.getNewWindowPos(
            Margin(0.0f, guiRenderer.m_MarginDefault, guiRenderer.m_MarginDefault, guiRenderer.m_MarginDefault),
            windowSize, Alignment::TopRight);
        ImGui::SetNextWindowPos(newPos, ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(windowSize, ImGuiCond_FirstUseEver);
        ImGui::Begin("Details", nullptr, guiRenderer.m_WindowFlags);
        if (guiRenderer.m_CurrentSelectedID != ECS::INVALID_ENTITY_ID)
        {
            const ECS::EntityHandle handle = {
                guiRenderer.m_CurrentSelectedID,
                guiRenderer.m_Registry
            };
            Component::renderEntityComponentUI(handle);
            drawAddComponentButton(handle);
        }
        ImGui::End();
    }
}
