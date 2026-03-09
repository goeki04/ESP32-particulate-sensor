#pragma once

namespace Andromeda {
	struct GLtexture
	{
		GLuint id = 0;
		int w = 0;
		int h = 0;

		GLtexture() = default;
		GLtexture(const GLtexture&) = delete;
		GLtexture& operator=(const GLtexture&) = delete;
		GLtexture(GLtexture&& other) noexcept
		{
			*this = std::move(other);
		}

		GLtexture& operator=(GLtexture&& other) noexcept
		{
			if (this != &other) {
				destroy();
				id = other.id;
				w = other.w;
				h = other.h;
				other.id = 0;
				other.w = other.h = 0;
			}
			return *this;
		}

		~GLtexture() { destroy(); }

		void destroy()
		{
			if (id != 0) {
				if (SDL_GL_GetCurrentContext()) {
					glDeleteTextures(1, &id);
				}
				id = 0;
			}
		}
	};
}