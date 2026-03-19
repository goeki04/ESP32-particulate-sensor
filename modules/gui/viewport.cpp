
#include "panels.h"
#include "a_math.hpp"
#include "gui_renderer.h"
void Andromeda::Gui::Panels::drawViewportGUI(GuiRenderer& guiRenderer,ViewportDrawInfo& drawInfo)
{
    ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;
    guiRenderer.m_ViewportPos = guiRenderer.getViewportWindowPos();
    vec2 vpSize = guiRenderer.getViewportWindowSize();
    ImVec2 viewportSize = ImVec2(vpSize.x,vpSize.y);
    ImGui::SetNextWindowPos(guiRenderer.m_ViewportPos, ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(viewportSize, ImGuiCond_FirstUseEver);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("Viewport", 0, windowFlags);
    ImVec2 currentSize = ImGui::GetContentRegionAvail();
    GuiRenderer::s_ViewportSize = currentSize;
    if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
    {
        ImGui::SetWindowFocus();
    }
    assert(drawInfo.camData && "drawInfo.camData is nullptr in Andromeda::Gui::Panels::drawViewportGui");
    drawInfo.camData->m_ImGuiMouseX = -1.0f;
    drawInfo.camData->m_ImGuiMouseY = -1.0f;
    ImGui::Image((void*)(intptr_t)drawInfo.postProcessingFboTexture, currentSize, ImVec2(0, 1), ImVec2(1, 0));

    ImVec2 rectMin = ImGui::GetItemRectMin();
    ImVec2 rectMax = ImGui::GetItemRectMax();
    ImVec2 rectSize = ImVec2(rectMax.x - rectMin.x, rectMax.y - rectMin.y);
    drawInfo.camData->m_ViewportSize = vec2(viewportSize.x, viewportSize.y);
    drawInfo.camData->m_ViewportPos = vec2(guiRenderer.m_ViewportPos.x, guiRenderer.m_ViewportPos.y);
    ImVec2 mousePos = ImGui::GetMousePos();
    ImVec2 rel = ImVec2(mousePos.x - rectMin.x, mousePos.y - rectMin.y);
    bool hovered = ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem);
    GuiRenderer::s_ViewportFocused = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows);
    drawInfo.camData->m_HasValidPickRay = hovered;

    if (hovered) {
        drawInfo.camData->m_ImGuiMouseX = rel.x;
        drawInfo.camData->m_ImGuiMouseY = rel.y;
        drawInfo.camData->m_framebufferSize = glm::vec2(rectSize.x, rectSize.y);

        drawInfo.camData->m_ImGuiMouseX = rel.x;
        drawInfo.camData->m_ImGuiMouseY = rel.y;
    }
    else {
        drawInfo.camData->m_ImGuiMouseX = drawInfo.camData->m_ImGuiMouseY = -1.0f;
    }

    ImGui::End();
    ImGui::PopStyleVar();
}