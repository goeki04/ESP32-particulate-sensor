#include "pch.h"
#include "panels.h"
#include "GuiRenderer.h"
#include "camera.h"
#include "ResourceManager.h"
void Andromeda::Gui::Panels::drawViewportGUI(GuiRenderer& guiRenderer,unsigned int framebufferTexture, 
    ImVec2 framebufferSize, float* ImGuiMouseX, float* ImGuiMouseY)
{
    guiRenderer.m_ResourceManager->m_Cam;
    Andromeda::Camera& cam = guiRenderer.m_ResourceManager->m_Cam;
    ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;
    guiRenderer.m_ViewportPos = guiRenderer.getViewportWindowPos();
    ImVec2 viewportSize = guiRenderer.getViewportWindowSize();
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

    if (ImGuiMouseX) *ImGuiMouseX = -1.0f;
    if (ImGuiMouseY) *ImGuiMouseY = -1.0f;

    ImGui::Image((void*)(intptr_t)framebufferTexture, currentSize, ImVec2(0, 1), ImVec2(1, 0));

    ImVec2 rectMin = ImGui::GetItemRectMin();
    ImVec2 rectMax = ImGui::GetItemRectMax();
    ImVec2 rectSize = ImVec2(rectMax.x - rectMin.x, rectMax.y - rectMin.y);
    cam.m_ViewportSize = glm::vec2(viewportSize.x, viewportSize.y);
    cam.m_ViewportPos = glm::vec2(guiRenderer.m_ViewportPos.x, guiRenderer.m_ViewportPos.y);
    ImVec2 mousePos = ImGui::GetMousePos();
    ImVec2 rel = ImVec2(mousePos.x - rectMin.x, mousePos.y - rectMin.y);

    bool hovered = ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem);
    GuiRenderer::s_ViewportFocused = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows);
    cam.m_HasValidPickRay = hovered;

    cam.m_HasValidPickRay = hovered;

    if (hovered) {
        cam.m_ImGuiMouseX = rel.x;
        cam.m_ImGuiMouseY = rel.y;
        cam.m_framebufferSize = glm::vec2(rectSize.x, rectSize.y);
        cam.m_CursorToWorldRay = cam.cursorToWorldRay();

        if (ImGuiMouseX) *ImGuiMouseX = rel.x;
        if (ImGuiMouseY) *ImGuiMouseY = rel.y;
    }
    else {
        cam.m_ImGuiMouseX = cam.m_ImGuiMouseY = -1.0f;
    }

    ImGui::End();
    ImGui::PopStyleVar();
}