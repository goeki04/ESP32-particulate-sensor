#pragma once
namespace Gui {
	class GuiRenderer;
}
namespace Gui::Panels {

	void drawNavBar(GuiRenderer& guiRenderer);
	void drawDeviceHierarchy(GuiRenderer& guiRenderer);
	void drawDeviceBrowser(GuiRenderer& guiRenderer);
	void drawViewportGUI(GuiRenderer& guiRenderer,unsigned int framebufferTexture, ImVec2 framebufferSize, float* ImGuiMouseX, float* ImGuiMouseY);
}