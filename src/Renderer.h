#pragma once

class Renderer {
public:
	SDL_Renderer* m_SDLRenderer = NULL;
	void draw();
	virtual void RenderImGui() {};
};