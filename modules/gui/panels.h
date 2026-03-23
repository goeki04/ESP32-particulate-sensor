#pragma once
#include "a_primitives.hpp"
#include "a_guiTypes.hpp"
namespace Andromeda::Gui {
	class GuiRenderer;
}
namespace Andromeda::amath {
	struct CameraData;
}
namespace Andromeda::Gui::Panels {
	void drawNavBar(GuiRenderer& guiRenderer);
	void drawDeviceHierarchy(GuiRenderer& guiRenderer);
	void deviceBrowserPicking(GuiRenderer& guiRenderer, const amath::CameraData* cam, ECS::ComponentRegistry* registry);
	void drawDeviceBrowser(GuiRenderer& guiRenderer);
	void drawViewportGUI(GuiRenderer& guiRenderer, ViewportDrawInfo& drawInfo);
}