#include "panels.h"
#include "gui_renderer.h"
#include "component_ui.hpp"
namespace Andromeda::Gui::Panels {
    void drawDetails(const GuiRenderer& guiRenderer)
    {
        const float height = static_cast<float>(guiRenderer.m_WindowHeight) - guiRenderer.m_MenuBarHeight - guiRenderer.m_MarginDefault * 2.0f;
        const auto windowSize = ImVec2(guiRenderer.m_WidgetWidth, height);
        const ImVec2 newPos = guiRenderer.getNewWindowPos(Margin(0.0f, guiRenderer.m_MarginDefault, guiRenderer.m_MarginDefault, guiRenderer.m_MarginDefault), windowSize, Alignment::TopRight);
        ImGui::SetNextWindowPos(newPos, ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(windowSize, ImGuiCond_FirstUseEver);
        ImGui::Begin("Details", nullptr, guiRenderer.m_WindowFlags);
        if (guiRenderer.m_CurrentSelectedID != 0)
        {
        }
        else
        {
            ImGui::Text("No entity selected.");
        }
        ImGui::End();
    }
}