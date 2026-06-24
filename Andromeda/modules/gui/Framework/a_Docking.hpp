#pragma once

/**
 * @file a_Docking.hpp
 * @brief Helper for constructing the editor's default dockspace layout.
 */

#include "imgui.h"
namespace Andromeda::Gui {
	/**
	 * @class DockBuilder
	 * @brief Builds the initial arrangement of editor panels within an ImGui dockspace.
	 */
	class DockBuilder {
	public:
		/**
		 * @brief Splits the given dockspace into the engine's default panel layout.
		 *
		 * @details Defines where each panel docks on first launch (e.g. hierarchy on the
		 *          left, inspector on the right, viewport in the center). Typically called
		 *          once when no saved ImGui layout (.ini) exists yet.
		 *
		 * @param dockspace_id The ImGui ID of the root dockspace node to partition.
		 */
		static void setDefaultLayout(ImGuiID dockspace_id);
	};
}