#include "a_Docking.hpp"
#include "imgui_internal.h"
namespace Andromeda::Gui {
    void DockBuilder::setDefaultLayout(ImGuiID dockspace_id)
    {
            ImGui::DockBuilderRemoveNode(dockspace_id);
            ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace);
            ImGui::DockBuilderSetNodeSize(dockspace_id, ImGui::GetMainViewport()->Size);
            ImGuiID dock_id_main = dockspace_id;
            ImGuiID dock_id_left, dock_id_right, dock_id_bottom, dock_id_bottom_left;

            dock_id_right = ImGui::DockBuilderSplitNode(dock_id_main, ImGuiDir_Right, 0.20f, nullptr, &dock_id_main);

            dock_id_bottom = ImGui::DockBuilderSplitNode(dock_id_main, ImGuiDir_Down, 0.35f, nullptr, &dock_id_main);

            dock_id_bottom_left = ImGui::DockBuilderSplitNode(dock_id_bottom, ImGuiDir_Left, 0.20f, nullptr, &dock_id_bottom);

            dock_id_left = ImGui::DockBuilderSplitNode(dock_id_main, ImGuiDir_Left, 0.20f, nullptr, &dock_id_main);

            ImGui::DockBuilderDockWindow("Details", dock_id_right);
            ImGui::DockBuilderDockWindow("Hierarchy", dock_id_left);
            ImGui::DockBuilderDockWindow("Chart", dock_id_bottom_left);
            ImGui::DockBuilderDockWindow("Browser", dock_id_bottom);
            ImGui::DockBuilderDockWindow("Console", dock_id_bottom);
            ImGui::DockBuilderDockWindow("Viewport", dock_id_main);

            ImGui::DockBuilderFinish(dockspace_id);
    }
}