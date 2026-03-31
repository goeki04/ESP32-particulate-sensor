#include "panels.h"
#include "gui_renderer.h"
namespace Andromeda::Gui::Panels {
    void drawDetails(GuiRenderer& guiRenderer)
    {
        ImVec2 windowSize = ImVec2(guiRenderer.m_WidgetWidth, guiRenderer.m_WindowHeight - guiRenderer.m_MenuBarHeight - guiRenderer.m_MarginDefault * 2);
        ImVec2 newPos = guiRenderer.getNewWindowPos(Margin(0.0f, guiRenderer.m_MarginDefault, guiRenderer.m_MarginDefault, guiRenderer.m_MarginDefault), windowSize, Alignment::TopRight);
        ImGui::SetNextWindowPos(newPos, ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(windowSize, ImGuiCond_FirstUseEver);
        ImGui::Begin("Details", 0, guiRenderer.m_WindowFlags);

        ImGui::End();
    }
}