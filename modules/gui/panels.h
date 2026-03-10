#pragma once
namespace Andromeda::Gui {
	class GuiRenderer;
}
namespace Andromeda::Gui::Panels {
	void drawNavBar(GuiRenderer& guiRenderer);
	void drawDeviceHierarchy(GuiRenderer& guiRenderer);
	void deviceBrowserPicking(GuiRenderer& guiRenderer);
	void drawDeviceBrowser(GuiRenderer& guiRenderer);
	void drawViewportGUI(GuiRenderer& guiRenderer,unsigned int framebufferTexture, ImVec2 framebufferSize, float* ImGuiMouseX, float* ImGuiMouseY);
}