#pragma once
#include "a_GLcubemap.hpp"
#include <GL/glew.h>
#include "a_primitives.hpp"
namespace Andromeda {
	class CubemapGL {
	public:
		static void CubemapTextureUploadGL(CubemapData& data) {
			GLuint tempID;
			glGenTextures(1, &tempID);
			data.textureID = tempID;
			glBindTexture(GL_TEXTURE_CUBE_MAP,  data.textureID);

			for (u32 i = 0; i < 6; ++i)
			{
				if (data.pixelData[i]) {
					glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB,
								 data.width, data.height, 0, GL_RGB, GL_UNSIGNED_BYTE,
								 data.pixelData[i]);
				}
			}

			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
			data.freePixelData();
			data.pixelData.fill(nullptr);
		}
	};
}