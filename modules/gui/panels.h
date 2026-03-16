#pragma once
#include "a_primitives.hpp"
namespace Andromeda::Gui {
	class GuiRenderer;
}
namespace Andromeda::amath {
	struct CameraData;
}
namespace Andromeda::Gui::Panels {
	void drawNavBar(GuiRenderer& guiRenderer);
	void drawDeviceHierarchy(GuiRenderer& guiRenderer);
	void deviceBrowserPicking(GuiRenderer& guiRenderer);
	void drawDeviceBrowser(GuiRenderer& guiRenderer);
	void drawViewportGUI(amath::CameraData& camData,GuiRenderer& guiRenderer, u32 framebufferTexture, vec2 framebufferSize, float* ImGuiMouseX, float* ImGuiMouseY);
}