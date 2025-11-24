#pragma once

class Renderer {
public:
	
	void draw();
protected:
	virtual void ImGuiRenderer() {
		std::cout << "Hello there" << std::endl;
	};
};