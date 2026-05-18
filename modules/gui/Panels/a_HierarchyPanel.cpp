#include "a_HierarchyPanel.hpp"
#include <imgui.h>
#include "a_components.hpp"
#include "a_SelectionContext.hpp"
#include "a_EditorContext.hpp"
#include "a_registry.hpp"
#include "a_event_manager.hpp"
#include "a_primitiveGenerator.hpp"
#include "resource_manager.h"
#include "scene.hpp"

namespace Andromeda::Gui
{
    namespace
    {
        constexpr glm::vec3 DEFAULT_SPAWN_POS{ 0.0f, 0.0f, 0.0f };
        constexpr glm::vec3 GROUND_SPAWN_POS{ 0.0f, 0.01f, 0.0f };
        template<typename GeneratorFunc>
        void spawnPrimitive(EditorContext& ctx, const char* name, const glm::vec3& position, GeneratorFunc generate)
        {
            Andromeda::Mesh mesh;
            generate(mesh);

            const u32 meshID = ctx.resourceManager->registerCustomMesh(std::move(mesh), std::string("Default_") + name);

            ECS::Component::Transform t;
            t.position = position;
            ctx.sceneManager->addEntity(meshID, name, t);
        }

        void DrawSearchBar(ImTextureID iconTex, char* buffer, size_t bufferSize)
        {
            ImDrawList* drawList = ImGui::GetWindowDrawList();
            ImVec2 startPos = ImGui::GetCursorScreenPos();
            float width = ImGui::GetContentRegionAvail().x;
            float height = ImGui::GetFrameHeight(); 

            const ImU32 frameColor = ImGui::GetColorU32(ImGuiCol_FrameBg);
            const float rounding = ImGui::GetStyle().FrameRounding;
            drawList->AddRectFilled(startPos, ImVec2(startPos.x + width, startPos.y + height), frameColor, rounding);
            const float iconPadding = 10.0f;
            const float iconSize = height - (iconPadding * 2.0f);
            ImVec2 iconMin = ImVec2(startPos.x + iconPadding, startPos.y + iconPadding);
            ImVec2 iconMax = ImVec2(iconMin.x + iconSize, iconMin.y + iconSize);

            if (iconTex) {
                drawList->AddImage(iconTex, iconMin, iconMax);
            }

            float textOffset = iconSize + (iconPadding * 2.0f);
            ImGui::SetCursorScreenPos(ImVec2(startPos.x + textOffset, startPos.y));

            ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0, 0, 0, 0));
            ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));

            ImGui::SetNextItemWidth(width - textOffset);
            ImGui::InputTextWithHint("##searchHierarchy", "Search Entities...", buffer, bufferSize);

            ImGui::PopStyleColor(2);
        }

        bool containsIgnoreCase(const std::string& haystack, const std::string& needle) {
            if (needle.empty()) return true;
            auto it = std::search(
                haystack.begin(), haystack.end(),
                needle.begin(), needle.end(),
                [](char ch1, char ch2) { return std::tolower(static_cast<unsigned char>(ch1)) == std::tolower(static_cast<unsigned char>(ch2)); }
            );
            return it != haystack.end();
        }
    }

    const std::string& HierarchyPanel::getEntityName(ECS::EntityHandle& handle)
    {
        static const std::string unnamed = "Unnamed";
        return handle.has<ECS::Component::Tag>()
            ? handle.get<ECS::Component::Tag>().name
            : unnamed;
    }

    void HierarchyPanel::selectEntity(EditorContext& ctx, const ECS::Entity entity)
    {
        if (entity == ctx.selection->getSelectedEntity()) return;

        ctx.selection->setSelectedEntity(entity);
        EventManager::getInstance().Dispatch(EventType::OnSceneObjectSelected, SceneObjectSelected(static_cast<u32>(entity)));
    }

    void HierarchyPanel::drawEntityNode(EditorContext& ctx, const ECS::Entity e)
    {
        const bool isSelected = (ctx.selection->getSelectedEntity() == e);
        ECS::EntityHandle handle = { e, ctx.registry };

        ImGui::PushID(static_cast<i32>(e));

        if (isSelected) ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.5f, 0.0f, 1.0f));

        if (ImGui::Selectable(getEntityName(handle).c_str(), isSelected)) {
            selectEntity(ctx, e);
        }

        if (isSelected) ImGui::PopStyleColor();

        if (ImGui::BeginPopupContextItem())
        {
            selectEntity(ctx, e);
            ImGui::TextDisabled("Entity Actions");
            ImGui::Separator();

            if (ImGui::MenuItem("Delete"))
            {
                ctx.registry->destroyEntity(e);
                if (isSelected) {
                    ctx.selection->setSelectedEntity(ECS::INVALID_ENTITY_ID);
                    EventManager::getInstance().Dispatch(EventType::OnSceneObjectSelected, SceneObjectSelected(static_cast<u32>(ECS::INVALID_ENTITY_ID)));
                }
            }
            ImGui::EndPopup();
        }
        ImGui::PopID();
    }

    void HierarchyPanel::drawContextMenu(EditorContext& ctx)
    {
        if (!ImGui::BeginPopup("HierarchyContext")) return;

        ImGui::TextDisabled("Create");
        ImGui::Separator();

        if (ImGui::MenuItem("Cube"))   spawnPrimitive(ctx, "Cube", DEFAULT_SPAWN_POS, PrimitiveGenerator::generateCube);
        if (ImGui::MenuItem("Plane"))  spawnPrimitive(ctx, "Plane", GROUND_SPAWN_POS, PrimitiveGenerator::generatePlane);
        if (ImGui::MenuItem("Sphere")) spawnPrimitive(ctx, "Sphere", DEFAULT_SPAWN_POS, PrimitiveGenerator::generateSphere);

        ImGui::Separator();

        if (ImGui::MenuItem("Create Empty"))
        {
            ECS::Component::Transform t;
            ctx.sceneManager->addEntity(0, "GameObject", t);
        }

        ImGui::EndPopup();
    }

    void HierarchyPanel::onGuiRender(EditorContext& ctx)
    {
        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 0.0f);
        if (ImGui::Begin(m_Name, &m_IsOpen))
        {
            ImTextureID texID = reinterpret_cast<ImTextureID>(m_SearchTextureHandle);
            DrawSearchBar(texID, m_SearchQuery, IM_ARRAYSIZE(m_SearchQuery));
            const auto& entities = ctx.registry->getAllEntities();
            ImGui::SameLine();
            ImGui::AlignTextToFramePadding();
            ImGui::TextDisabled("Create");

            std::string searchStr = m_SearchQuery;
            std::vector<ECS::Entity> filteredEntities;
            filteredEntities.reserve(entities.size());
            bool triggerPopup = false;
            for (const ECS::Entity e : entities) {
                ECS::EntityHandle handle{ e, ctx.registry };
                const std::string& name = getEntityName(handle);

                if (containsIgnoreCase(name, searchStr)) {
                    filteredEntities.push_back(e);
                }
            }
            if (ImGui::BeginChild("HierarchyList", ImVec2(0.0f, 0.0f), true, ImGuiWindowFlags_None))
            {
                ImGuiListClipper clipper;
                clipper.Begin(static_cast<int>(filteredEntities.size()));
                while (clipper.Step()) {
                    for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; ++i) {
                        drawEntityNode(ctx, filteredEntities[i]);
                    }
                }

                if (ImGui::IsWindowHovered() && !ImGui::IsAnyItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
                    triggerPopup = true; 
                }

                ImGui::EndChild();
            }


            if (triggerPopup) {
                ImGui::OpenPopup("HierarchyContext");
            }

            drawContextMenu(ctx);
        }
        ImGui::End();
        ImGui::PopStyleVar();
    }
    void HierarchyPanel::initPanel(EditorContext& ctx)
    {
        u32 id = ctx.resourceManager->getEditorIconID("search");
        m_SearchTextureHandle = reinterpret_cast<void*>(static_cast<intptr_t>(id));
    }
}