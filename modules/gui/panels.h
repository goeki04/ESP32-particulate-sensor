#pragma once
#include "a_primitives.hpp"
namespace Andromeda::Gui {
	class GuiRenderer;
}
namespace Andromeda {
	class Camera;
}
namespace Andromeda::Gui::Panels {
	void drawNavBar(GuiRenderer& guiRenderer);
	void drawDeviceHierarchy(GuiRenderer& guiRenderer);
	void deviceBrowserPicking(GuiRenderer& guiRenderer);
	void drawDeviceBrowser(GuiRenderer& guiRenderer);
	void drawViewportGUI(Camera& cam,GuiRenderer& guiRenderer, u32 framebufferTexture, vec2 framebufferSize, float* ImGuiMouseX, float* ImGuiMouseY);
}