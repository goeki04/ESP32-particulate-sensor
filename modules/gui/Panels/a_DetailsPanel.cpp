#include "a_DetailsPanel.hpp"
#include "imgui.h"
#include <algorithm>
#include <string>
#include "generated_components.hpp"
#include "generated_component_names.hpp"
#include "a_EditorContext.hpp"
#include "a_SelectionContext.hpp"
#include "component_ui.hpp"
namespace Andromeda::Gui {
    
    void DetailsPanel::onGuiRender(EditorContext& ctx)
    {
        if (!ImGui::Begin(m_Name, &m_IsOpen)) {
            ImGui::End();
            return;
        }
        ECS::Entity entity = ctx.selection->getSelectedEntity();
        if (entity != ECS::INVALID_ENTITY_ID) {
            ECS::EntityHandle handle = { entity, ctx.registry };

            Component::renderEntityComponentUI(handle,ctx.resourceManager,m_ActiveUndoState);

            ImGui::Separator();
            ImGui::Spacing();
            drawAddComponentButton(handle);
        }
        else {
            ImGui::TextDisabled("No Entity selected");
        }

        ImGui::End();
    }

    bool DetailsPanel::passesFilter(std::string_view name, const char* filter)
    {
        if (filter == nullptr || filter[0] == '\0') {
            return true;
        }

        auto it = std::search(
            name.begin(), name.end(),
            filter, filter + std::strlen(filter),
            [](char a, char b) {
                return std::tolower(static_cast<unsigned char>(a)) ==
                    std::tolower(static_cast<unsigned char>(b));
            }
        );

        return it != name.end();
    }

    void DetailsPanel::renderComponentList(ECS::EntityHandle handle, const char* filter)
    {
        std::apply([&]<typename... Args>(Args&&...)
        {
            ([&]
                {
                    if (!handle.has<Args>())
                    {
                        const std::string_view name = ECS::Component::get_component_name<Args>();

                        if (passesFilter(name, filter))
                        {

                            if (ImGui::Selectable(name.data()))
                            {
                                handle.add<Args>({});
                                ImGui::CloseCurrentPopup();
                            }
                        }
                    }
                }(), ...);
        }, ECS::Component::ComponentDirectory{});
    }

    void DetailsPanel::renderComponentSearchPopup(const ECS::EntityHandle handle, const float width)
    {
        ImGui::SetNextWindowSize(ImVec2(width, 0));

        if (ImGui::BeginPopup("ComponentSearchPopup")) {
            static char searchBuffer[64] = "";

            if (ImGui::IsWindowAppearing()) {
                ImGui::SetKeyboardFocusHere();
                searchBuffer[0] = '\0';
            }

            ImGui::InputTextWithHint("##Filter", "Search...", searchBuffer, IM_ARRAYSIZE(searchBuffer));
            ImGui::Separator();

            renderComponentList(handle, searchBuffer);

            ImGui::EndPopup();
        }
    }
    void DetailsPanel::drawAddComponentButton(const ECS::EntityHandle handle)
    {
        const ImVec2 buttonPos = ImGui::GetCursorScreenPos();
        const float buttonWidth = ImGui::GetContentRegionAvail().x;

        if (ImGui::Button("Add Component", ImVec2(buttonWidth, 0))) {
            ImGui::OpenPopup("ComponentSearchPopup");
        }

        const ImVec2 popupPos = ImVec2(buttonPos.x, buttonPos.y + ImGui::GetFrameHeightWithSpacing());

        ImGui::SetNextWindowPos(popupPos);

        renderComponentSearchPopup(handle, buttonWidth);
    }
}