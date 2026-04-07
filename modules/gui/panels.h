#pragma once
#include "a_primitives.hpp"
#include "a_guiTypes.hpp"
#include <string>
#include "a_registry.hpp"
namespace Andromeda::Gui {
	class GuiRenderer;
}
namespace Andromeda::amath {
	struct CameraData;
}
namespace Andromeda::Gui::Panels {
	void drawNavBar(GuiRenderer& guiRenderer);
	std::string getEntityName(ECS::EntityHandle handle);
	void selectEntity(GuiRenderer& guiRenderer, ECS::Entity entity);
	void drawEntityNode(GuiRenderer& guiRenderer, ECS::Entity e);
	void drawDeviceHierarchy(GuiRenderer& guiRenderer);
	void deviceBrowserPicking(GuiRenderer& guiRenderer, const amath::CameraData* cam, ECS::ComponentRegistry* registry);
	void drawDeviceBrowser(GuiRenderer& guiRenderer);
	void drawDetails(const GuiRenderer& guiRenderer);
	void drawViewportGUI(GuiRenderer& guiRenderer, const ViewportDrawInfo& drawInfo);
}