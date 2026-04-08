#include "panels.h"
#include "a_math.hpp"
#include "gui_renderer.h"
namespace Andromeda::Gui {

    struct ViewportDimension {
        ImVec2 min;
        ImVec2 max;
        ImVec2 size;
    };

    void setOverlayStyle() {
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0, 0, 0, 0));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0));
    }

    void resetOverlayStyle() {
        ImGui::PopStyleColor(3);
        ImGui::PopStyleVar(3);
    }

    void drawWireframeControl(const u32& textureID, ImGuiWindowFlags flags) {
        const ImTextureID imguiID = (ImTextureID)(intptr_t)textureID;
        ImVec2 childSize = ImVec2(32, 32);
        ImVec2 screenPos = ImGui::GetCursorScreenPos();
        setOverlayStyle();

        bool clicked = false;
        if (ImGui::BeginChild("Toolbar", childSize, ImGuiChildFlags_None, flags)) {
            if (ImGui::ImageButton("wireframe", imguiID, childSize, ImVec2(0, 0), ImVec2(1, 1))) {
                clicked = true;
            }
        }

        bool isHovered = ImGui::IsItemHovered();
        bool isActive = ImGui::IsItemActive();
        ImGui::EndChild();

        ImDrawList* drawList = ImGui::GetWindowDrawList();
        ImVec2 center = ImVec2(screenPos.x + childSize.x * 0.5f, screenPos.y + childSize.y * 0.5f);
        float radius = (childSize.x * 0.5f) + 6.0f;

        ImVec4 color = ImVec4(0.1f, 0.1f, 0.1f, 0.6f);
        if (isActive)      color = ImVec4(0.4f, 0.4f, 0.4f, 0.8f);
        else if (isHovered) color = ImVec4(0.25f, 0.25f, 0.25f, 0.7f);

        drawList->AddCircleFilled(center, radius, ImGui::GetColorU32(color));

        resetOverlayStyle();

        if (clicked) {
        }
    }

    void drawViewportOverlay(ImVec2 rectMax, ImVec2 rectMin, const u32& textureID) {
        float padding = 10.0f;
        ImVec2 childSize = ImVec2(32.0f, 32.0f);
        auto overlayPos = ImVec2(rectMax.x - padding, rectMin.x + padding);
        ImVec2 startPos = ImGui::GetCursorStartPos();
        float xPos = startPos.x + ImGui::GetContentRegionAvail().x - childSize.x - padding;
        float yPos = startPos.y + padding;
        ImGui::SetCursorPos(ImVec2(xPos,yPos));
        ImGui::SetNextWindowBgAlpha(0.35f);
        ImGuiWindowFlags overlayFlags = ImGuiWindowFlags_NoDecoration |
            ImGuiWindowFlags_NoDocking |
            ImGuiWindowFlags_AlwaysAutoResize |
            ImGuiWindowFlags_NoSavedSettings |
            ImGuiWindowFlags_NoFocusOnAppearing |
            ImGuiWindowFlags_NoBackground |
            ImGuiWindowFlags_NoNav |
            ImGuiWindowFlags_NoMove;
        drawWireframeControl(textureID,overlayFlags);
    }

    void updateImGuiMousePos(const ViewportDimension& vpDimension, Gui::GuiRenderer& guiRenderer,const Gui::ViewportDrawInfo& drawInfo) {
        auto* cam = drawInfo.camData;
        cam->viewportSize = vec2(vpDimension.size.x, vpDimension.size.y);
        cam->viewportPos = vec2(vpDimension.min.x, vpDimension.min.y);

        ImVec2 mousePos = ImGui::GetMousePos();
        bool hovered = ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem);

        Gui::GuiRenderer::s_ViewportFocused = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows);
        guiRenderer.m_ViewportHovered = hovered;
        cam->hasValidPickRay = hovered;

        if (hovered) {
            cam->imGuiMouseX = mousePos.x - vpDimension.min.x;
            cam->imGuiMouseY = mousePos.y - vpDimension.min.y;
            cam->framebufferSize = glm::vec2(vpDimension.size.x, vpDimension.size.y);
        }
        else {
            cam->imGuiMouseX = cam->imGuiMouseY = -1.0f;
        }
    }
    void drawViewportImage(Gui::GuiRenderer& guiRenderer, const Gui::ViewportDrawInfo& drawInfo)
    {
        const ImVec2 currentSize = ImGui::GetContentRegionAvail();
        Gui::GuiRenderer::s_ViewportSize = currentSize;

        ImGui::Image((void*)static_cast<intptr_t>(drawInfo.postProcessingFboTexture), 
                     currentSize, ImVec2(0, 1), ImVec2(1, 0));

        ImVec2 rectMin = ImGui::GetItemRectMin();
        ImVec2 rectMax = ImGui::GetItemRectMax();
   
        ViewportDimension vpDimension{
            rectMin,
            rectMax,
            currentSize
        };
        updateImGuiMousePos(vpDimension, guiRenderer, drawInfo);

        drawViewportOverlay(rectMax, rectMin, guiRenderer.m_ResourceManager->getEditorIconID("box"));
    }

    void handleViewportInput(const Gui::ViewportDrawInfo& drawInfo)
    {
        if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
        {
            drawInfo.camData->canRotate = true;
            ImGui::SetWindowFocus();
        }

        if (ImGui::IsMouseReleased(ImGuiMouseButton_Right))
        {
            drawInfo.camData->canRotate = false;
        }
    }

    void Gui::Panels::drawViewportGUI(GuiRenderer& guiRenderer, const ViewportDrawInfo& drawInfo)
    {
        constexpr ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;

        guiRenderer.m_ViewportPos = guiRenderer.getViewportWindowPos();
        const vec2 vpSize = guiRenderer.getViewportWindowSize();

        ImGui::SetNextWindowPos(guiRenderer.m_ViewportPos, ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(vpSize.x, vpSize.y), ImGuiCond_FirstUseEver);

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        if (ImGui::Begin("Viewport", nullptr, windowFlags))
        {
            handleViewportInput(drawInfo);
            drawViewportImage(guiRenderer, drawInfo);
        }
        ImGui::End();
        ImGui::PopStyleVar();
    }
}