#pragma once
namespace Andromeda::Gui {
	class GuiRenderer;
}
namespace Andromeda::Gui::Panels {
	void drawNavBar(Andromeda::Gui::GuiRenderer& guiRenderer);
	void drawDeviceHierarchy(Andromeda::Gui::GuiRenderer& guiRenderer);
	void drawDeviceBrowser(Andromeda::Gui::GuiRenderer& guiRenderer);
	void drawViewportGUI(Andromeda::Gui::GuiRenderer& guiRenderer,unsigned int framebufferTexture, ImVec2 framebufferSize, float* ImGuiMouseX, float* ImGuiMouseY);
}