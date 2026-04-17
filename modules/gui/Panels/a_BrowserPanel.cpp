#include "a_BrowserPanel.hpp"
#include "a_EditorContext.hpp"
#include "a_ImGuiOperators.hpp"
#include <cmath>
#include "scene.hpp"
namespace Andromeda::Gui {

    void BrowserPanel::onImGuiRender(EditorContext& ctx) {
        if (ImGui::Begin(m_Name, &m_IsOpen)) {
            renderSearch(ctx);
            renderGrid(ctx);
        }
        ImGui::End();
    }

    void BrowserPanel::renderSearch(EditorContext& ctx) {
        ImGui::InputTextWithHint("##search", "Search components...", m_SearchQuery, IM_ARRAYSIZE(m_SearchQuery));
        ImGui::Spacing();
    }

    void BrowserPanel::renderGrid(EditorContext& ctx) {
        const float availX = ImGui::GetContentRegionAvail().x;
        const float spacingX = ImGui::GetStyle().ItemSpacing.x;
        const ImVec2 iconSize = Tile::getIconSize();

        int perRow = static_cast<int>(std::floor((availX + spacingX) / (iconSize.x + spacingX)));
        if (perRow < 1) perRow = 1;

        const uint32_t itemCount = ctx.deviceProvider->getDeviceCount();
        bool anyTileClicked = false;
        int visibleCount = 0;

        for (int i = 0; i < (int)itemCount; ++i) {
            const auto& device = ctx.deviceProvider->getDeviceData(i);

            if (m_SearchQuery[0] != '\0' && device.name.find(m_SearchQuery) == std::string::npos)
                continue;

            if (handleTile(ctx, device, i)) {
                anyTileClicked = true;
            }

            if ((visibleCount + 1) % perRow != 0 && i < (int)itemCount - 1) {
                ImGui::SameLine();
            }
            visibleCount++;
        }
        if (ImGui::IsMouseClicked(0) && !anyTileClicked && ImGui::IsWindowHovered()) {
            ctx.state.selectedIdx = -1;
        }
    }

    bool BrowserPanel::handleTile(EditorContext& ctx, const Device& device, int idx) {
        ImGui::PushID(idx);
        ImGui::BeginGroup();

        Tile tile{ device, ctx.deviceProvider->getDeviceIconID(device.type), idx };
        ImVec2 labelSize = ImGui::CalcTextSize(device.name.c_str());
        tile.pMin = ImGui::GetCursorScreenPos();
        tile.totalSize = { Tile::getIconSize().x, Tile::getIconSize().y + ImGui::GetStyle().ItemSpacing.y + labelSize.y };
        tile.pMax = tile.pMin + Tile::getIconSize();

        ImGui::InvisibleButton("##tile_btn", tile.totalSize);

        tile.hovered = ImGui::IsItemHovered();
        tile.active = ImGui::IsItemActive();
        tile.clicked = ImGui::IsItemClicked(0);
        tile.dragged = ImGui::IsMouseDragging(0, 2.0f);
        tile.dragEnded = ImGui::IsItemDeactivated() && ImGui::IsMouseReleased(0);
        tile.selected = (ctx.state.selectedIdx == idx);

        if (tile.clicked) ctx.state.selectedIdx = idx;

        drawTileVisuals(ImGui::GetWindowDrawList(), tile);
        handleDragAndDrop(ctx, tile);

        ImGui::EndGroup();
        ImGui::PopID();

        return tile.clicked;
    }

    void BrowserPanel::drawTileVisuals(ImDrawList* dl, const Tile& tile) {
        if (tile.device.type == deviceType::DEFAULT) {
            dl->AddRectFilled(tile.pMin, tile.pMax, IM_COL32(45, 50, 70, 255), 4.0f);
        }
        else {
            dl->AddImage((ImTextureID)(intptr_t)tile.texID, tile.pMin, tile.pMax);
        }

        if (tile.selected) {
            dl->AddRect(tile.pMin, tile.pMax, IM_COL32(0, 120, 255, 255), 4.0f, 0, 2.5f);
            dl->AddRectFilled(tile.pMin, tile.pMax, IM_COL32(0, 120, 255, 40), 4.0f);
        }
        else if (tile.hovered) {
            dl->AddRect(tile.pMin, tile.pMax, IM_COL32(255, 255, 255, 100), 4.0f);
        }

        float textX = tile.pMin.x + (tile.totalSize.x - ImGui::CalcTextSize(tile.device.name.c_str()).x) * 0.5f;
        dl->AddText({ textX, tile.pMax.y + ImGui::GetStyle().ItemSpacing.y }, IM_COL32_WHITE, tile.device.name.c_str());
    }

    void BrowserPanel::handleDragAndDrop(EditorContext& ctx, const Tile& tile) {
        if (tile.dragged && tile.active) {
            ctx.state.hasLastHitpoint = false;
            glm::vec3 hit;
            if (ctx.cameraData->hasValidPickRay && amath::RayIntersectsXZPlane(ctx.cameraData->cursorToWorldRay, 0.0f, hit)) {
                ctx.state.lastHitPoint = hit;
                ctx.state.hasLastHitpoint = true;
            }

            ImVec2 previewSize = Tile::getIconSize() * 0.8f;
            ImGui::GetForegroundDrawList()->AddImage((ImTextureID)(intptr_t)tile.texID,
                ImGui::GetMousePos() - (previewSize * 0.5f),
                ImGui::GetMousePos() + (previewSize * 0.5f),
                { 0,0 }, { 1,1 }, IM_COL32(255, 255, 255, 150));
        }

        if (tile.dragEnded && ctx.cameraData->hasValidPickRay && ctx.state.hasLastHitpoint) {
            ECS::Component::Transform t;
            t.position = ctx.state.lastHitPoint;
            ctx.sceneManager->addEntity(tile.device.id, tile.device.name, t);
        }
    }
}