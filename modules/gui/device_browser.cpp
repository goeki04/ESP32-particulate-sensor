#include "panels.h"
#include "a_device.hpp"
#include "scene.hpp"
#include "gui_renderer.h"
#include "a_math.hpp"

namespace Andromeda::Gui::Panels {
    void deviceBrowserPicking(EditorContext& ctx) {
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

        int perRow = static_cast<i32>(floor((availX + spacingX) / (tileSize.x + spacingX)));
        if (perRow < 1) perRow = 1;
        const u32 itemCount = ctx.deviceProvider->getDeviceCount();
        bool any_tile_clicked = false;

        for (i32 idx = 0; idx < itemCount; ++idx)
        {
            const auto& deviceRecord = ctx.deviceProvider->getDeviceData(idx);
            u32 texID = ctx.deviceProvider->getDeviceIconID(deviceRecord.type);

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
                ctx.state.selectedIdx = idx;
                any_tile_clicked = true;
            }
            bool selected = (ctx.state.selectedIdx == idx);

            ImVec2 tileMin = pMin;
            auto tileMax = ImVec2(pMin.x + tileSize.x, pMin.y + tileSize.y);

            if (deviceRecord.type == deviceType::DEFAULT) {
                dl->AddRectFilled(tileMin, tileMax, IM_COL32(41, 46, 66, 255), 4.0f);
            }
            else {
                dl->AddImage(static_cast<ImTextureID>(static_cast<intptr_t>(texID)), tileMin, tileMax);
            }
            if (dragged && active) {
               ctx.state.hasLastHitpoint = false;
                
                if (glm::vec3 hitpoint; ctx.cameraData->hasValidPickRay &&
                    amath::RayIntersectsXZPlane(ctx.cameraData->cursorToWorldRay, 0.0f, hitpoint)) {
                    ctx.state.lastHitPoint = hitpoint;
                    ctx.state.hasLastHitpoint = true;
                }
                
                ImDrawList* fg = ImGui::GetForegroundDrawList();
                ImVec2 tileSizeDragged = tileSize * 0.75f;
                ImVec2 tMin = ImGui::GetMousePos() - tileSizeDragged * 0.5f;
                auto tMax = ImVec2(tMin + tileSizeDragged);
                fg->AddImage(static_cast<ImTextureID>(static_cast<intptr_t>(texID)), tMin, tMax);
            }
            
            if (dragEnded && ctx.cameraData->hasValidPickRay && ctx.state.hasLastHitpoint) {
                ECS::Component::Transform transform;
                transform.position = ctx.state.lastHitPoint;
                ctx.sceneManager->addEntity(deviceRecord.id, deviceRecord.name, transform);
            }
            if (hovered || active)
                dl->AddRect(tileMin, tileMax, IM_COL32(255, 255, 255, 60), 4.0f, 0, 1.5f);

            if (selected) {
                dl->AddRectFilled(tileMin, tileMax, IM_COL32(40, 120, 255, 80), 4.0f);
                dl->AddRect(tileMin, tileMax, IM_COL32(40, 120, 255, 200), 4.0f, 0, 2.0f);

                constexpr float glow = 3.0f;
                dl->AddRect(tileMin - ImVec2(glow, glow),
                    tileMax + ImVec2(glow, glow),
                    IM_COL32(40, 120, 255, 90),
                    6.0f, 0, 2.0f);
            }

            float textX = tileMin.x + (tileSize.x - labelSize.x) * 0.5f;
            float textY = tileMax.y + spacingY;
            dl->AddText(ImVec2(textX, textY), ImGui::GetColorU32(ImGuiCol_Text), label);

            if (int col = idx % perRow; col != perRow - 1)
                ImGui::SameLine(0.0f, spacingX);

            ImGui::PopID();
        }

        if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) &&
            !any_tile_clicked &&
            !ImGui::IsAnyItemHovered() &&
            ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup))
        {
            ctx.state.selectedIdx = -1;
        }
    }

    void drawDeviceBrowser(EditorContext& ctx)
    {
        if (ImGui::Begin("Device Browser", nullptr)){
            deviceBrowserPicking(ctx);
            ImGui::End();
        }
    }
}