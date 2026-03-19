#include "panels.h"
#include "a_device.hpp"
#include "gui_renderer.h"
namespace Andromeda::Gui::Panels {
    void deviceBrowserPicking(GuiRenderer& guiRenderer) {
        static char query[128] = "";
        ImGui::InputTextWithHint("##search", "Search components...", query, IM_ARRAYSIZE(query));
        ImGui::Spacing();

        ImDrawList* dl = ImGui::GetWindowDrawList();
        ImGuiStyle& style = ImGui::GetStyle();

        const float h = ImGui::GetFrameHeight();
        const ImVec2 tileSize(h * 2.0f, h * 2.0f);

        const float availX = ImGui::GetContentRegionAvail().x;
        const float spacingX = style.ItemSpacing.x;
        const float spacingY = style.ItemSpacing.y;

        int perRow = (int)floor((availX + spacingX) / (tileSize.x + spacingX));
        if (perRow < 1) perRow = 1;
        assert(guiRenderer.m_DeviceProvider && "DeviceProvider is nullptr in guiRenderer.cpp");
        const int itemCount = guiRenderer.m_DeviceProvider->getDeviceCount();
        bool any_tile_clicked = false;

        for (int idx = 0; idx < itemCount; ++idx)
        {
            const auto& deviceRecord = guiRenderer.m_DeviceProvider->getDeviceData(idx);
            u32 texID = guiRenderer.m_DeviceProvider->getDeviceIconID(deviceRecord.type);

            ImGui::PushID(idx);

            const char* label = deviceRecord.name.c_str();
            ImVec2 labelSize = ImGui::CalcTextSize(label);
            ImVec2 totalSize(tileSize.x, tileSize.y + spacingY + labelSize.y);

            ImVec2 pMin = ImGui::GetCursorScreenPos();
            ImGui::InvisibleButton("tile", totalSize);

            bool hovered = ImGui::IsItemHovered();
            bool active = ImGui::IsItemActive();
            bool clicked = ImGui::IsItemClicked(ImGuiMouseButton_Left);
            bool dragged = ImGui::IsMouseDragging(ImGuiMouseButton_Left, 2.0f);
            bool dragEnded = ImGui::IsItemDeactivated() && ImGui::IsMouseReleased(ImGuiMouseButton_Left);
            if (clicked) {
                guiRenderer.m_SelectedIdx = idx;
                any_tile_clicked = true;
            }
            bool selected = (guiRenderer.m_SelectedIdx == idx);

            ImVec2 tileMin = pMin;
            ImVec2 tileMax = ImVec2(pMin.x + tileSize.x, pMin.y + tileSize.y);

            if (deviceRecord.type == deviceType::DEFAULT) {
                dl->AddRectFilled(tileMin, tileMax, IM_COL32(41, 46, 66, 255), 4.0f);
            }
            else {
                dl->AddImage((ImTextureID)(intptr_t)texID, tileMin, tileMax);
            }
            if (dragged && active) {
                glm::vec3 hitpoint;
                guiRenderer.m_HasLastHitpoint = false;
                /*
                if (cam.m_HasValidPickRay && cam.RayIntersectsXZPlane(cam.m_CursorToWorldRay, 0.0f, hitpoint)) {
                    guiRenderer.m_LastHitPoint = hitpoint;
                    guiRenderer.m_HasLastHitpoint = true;
                }
                */
                ImDrawList* fg = ImGui::GetForegroundDrawList();
                ImVec2 tileSizeDragged = tileSize * 0.75f;
                ImVec2 tMin = ImGui::GetMousePos() - tileSizeDragged * 0.5f;
                ImVec2 tMax = ImVec2(tMin + tileSizeDragged);
                fg->AddImage((ImTextureID)(intptr_t)texID, tMin, tMax);
            }
            /*
            if (dragEnded && cam.m_HasValidPickRay && guiRenderer.m_HasLastHitpoint) {
                ECS::Component::Transform transform;
                transform.position = guiRenderer.m_LastHitPoint;
                guiRenderer.m_ResourceManager->addEntity(deviceRecord.id, deviceRecord.name, transform);
            }*/
            if (hovered || active)
                dl->AddRect(tileMin, tileMax, IM_COL32(255, 255, 255, 60), 4.0f, 0, 1.5f);

            if (selected) {
                dl->AddRectFilled(tileMin, tileMax, IM_COL32(40, 120, 255, 80), 4.0f);
                dl->AddRect(tileMin, tileMax, IM_COL32(40, 120, 255, 200), 4.0f, 0, 2.0f);

                const float glow = 3.0f;
                dl->AddRect(tileMin - ImVec2(glow, glow),
                    tileMax + ImVec2(glow, glow),
                    IM_COL32(40, 120, 255, 90),
                    6.0f, 0, 2.0f);
            }

            float textX = tileMin.x + (tileSize.x - labelSize.x) * 0.5f;
            float textY = tileMax.y + spacingY;
            dl->AddText(ImVec2(textX, textY), ImGui::GetColorU32(ImGuiCol_Text), label);

            int col = idx % perRow;
            if (col != perRow - 1)
                ImGui::SameLine(0.0f, spacingX);

            ImGui::PopID();
        }

        if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) &&
            !any_tile_clicked &&
            !ImGui::IsAnyItemHovered() &&
            ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup))
        {
            guiRenderer.m_SelectedIdx = -1;
        }
    }

    void drawDeviceBrowser(GuiRenderer& guiRenderer)
    {
        ImVec2 windowSize = ImVec2
        (guiRenderer.m_WindowWidth - guiRenderer.m_MarginDefault * 2 - 2 * guiRenderer.m_WidgetWidth - 100,
            guiRenderer.m_WindowHeight * 0.35f);

        ImVec2 newPos = guiRenderer.getNewWindowPos
        (Margin(guiRenderer.m_MarginDefault, 0, guiRenderer.m_MarginDefault, 0),
            windowSize, Alignment::CenterBottom);

        float windowPadding = windowSize.x * 0.1f;
        ImGui::SetNextWindowPos(newPos, ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(windowSize, ImGuiCond_FirstUseEver);
        if (ImGui::Begin("Device Browser", 0, guiRenderer.m_WindowFlags)){
            ImVec2 widgetSize = ImVec2(140, 0);
            deviceBrowserPicking(guiRenderer);
            ImGui::End();
        }   
    }
}