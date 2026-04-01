
#include "panels.h"
#include "a_math.hpp"
#include "gui_renderer.h"
void Andromeda::Gui::Panels::drawViewportGUI(GuiRenderer& guiRenderer, const ViewportDrawInfo& drawInfo)
{
    constexpr ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;
    guiRenderer.m_ViewportPos = guiRenderer.getViewportWindowPos();
    const vec2 vpSize = guiRenderer.getViewportWindowSize();
    const auto viewportSize = ImVec2(vpSize.x,vpSize.y);
    ImGui::SetNextWindowPos(guiRenderer.m_ViewportPos, ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(viewportSize, ImGuiCond_FirstUseEver);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("Viewport", nullptr, windowFlags);
    const ImVec2 currentSize = ImGui::GetContentRegionAvail();
    GuiRenderer::s_ViewportSize = currentSize;
    assert(drawInfo.camData && "drawInfo.camData is nullptr in Andromeda::Gui::Panels::drawViewportGui");
    if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
    {
        drawInfo.camData->canRotate = true;
        ImGui::SetWindowFocus();
    }

    if (ImGui::IsMouseReleased(ImGuiMouseButton_Right)) {
        drawInfo.camData->canRotate = false;
    }
   
    drawInfo.camData->imGuiMouseX = -1.0f;
    drawInfo.camData->imGuiMouseY = -1.0f;
    ImGui::Image((void*)static_cast<intptr_t>(drawInfo.postProcessingFboTexture), currentSize, ImVec2(0, 1), ImVec2(1, 0));

    ImVec2 rectMin = ImGui::GetItemRectMin();
    ImVec2 rectMax = ImGui::GetItemRectMax();
    ImVec2 rectSize = ImVec2(rectMax.x - rectMin.x, rectMax.y - rectMin.y);
    drawInfo.camData->viewportSize = vec2(rectSize.x, rectSize.y);
    drawInfo.camData->viewportPos = vec2(rectMin.x, rectMin.y);
    ImVec2 mousePos = ImGui::GetMousePos();
    ImVec2 rel = ImVec2(mousePos.x - rectMin.x, mousePos.y - rectMin.y);
    bool hovered = ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem);
    GuiRenderer::s_ViewportFocused = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows);
    drawInfo.camData->hasValidPickRay = hovered;

    if (hovered) {
        drawInfo.camData->imGuiMouseX = rel.x;
        drawInfo.camData->imGuiMouseY = rel.y;
        drawInfo.camData->framebufferSize = glm::vec2(rectSize.x, rectSize.y);

        drawInfo.camData->imGuiMouseX = rel.x;
        drawInfo.camData->imGuiMouseY = rel.y;
    }
    else {
        drawInfo.camData->imGuiMouseX = drawInfo.camData->imGuiMouseY = -1.0f;
    }

    ImGui::End();
    ImGui::PopStyleVar();
}