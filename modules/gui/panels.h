#pragma once
#include "a_primitives.hpp"
#include "a_guiTypes.hpp"
#include <string>

#include "a_EditorContext.hpp"
#include "a_registry.hpp"
namespace Andromeda::Gui {
	class GuiRenderer;
}
namespace Andromeda::amath {
	struct CameraData;
}
namespace Andromeda::Gui::Panels {
	void drawNavBar(EditorContext& context);
	std::string getEntityName(ECS::EntityHandle handle);
	void selectEntity(EditorContext& ctx, ECS::Entity entity);
	void drawEntityNode(EditorContext& ctx, ECS::Entity e);
	void drawDeviceBrowser(EditorContext& ctx);
	void drawDetails(const EditorContext& ctx);
	void drawViewportGUI(EditorContext& ctx, GuiRenderer& guiRenderer, const ViewportDrawInfo& drawInfo);
}