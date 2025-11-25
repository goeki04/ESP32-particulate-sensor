#include "Renderer.h"
#include "pch.h"

void Renderer::start()
{
	GLenum err = glewInit();
	if (err != GLEW_OK) {
		SDL_Log("Glew couldn't initialize successfully!");
	}
}

void Renderer::update()
{
}
