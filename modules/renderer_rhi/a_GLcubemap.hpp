#pragma once
#include "a_GLcubemap.hpp"
#include <GL/glew.h>
#include "a_primitives.hpp"
namespace Andromeda {
	class CubemapGL {
	public:
		static void CubemapTextureUploadGL(CubemapData& data) {
			if (data.isHDR)
			{
				createCubeMapTextureHDR(data);
			}
			else
			{
				createCubeMapTextureLDR(data);
			}

			data.freePixelData();
			data.pixelData.fill(nullptr);
		}

		static void createCubeMapTextureHDR(CubemapData& data)
		{
			GLuint tempID;
			glGenTextures(1, &tempID);
			data.textureID = tempID;
			glBindTexture(GL_TEXTURE_2D,  data.textureID);
			if (data.pixelData[0]) {
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F,
							 data.width, data.height, 0, GL_RGB, GL_FLOAT,
							 data.pixelData[0]);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			}
		}

		static void createCubeMapTextureLDR(CubemapData& data)
		{
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
		}

		static void AllocateEnvironmentMapTexture() {
			u32 envCubemap = 0;
			glGenTextures(1, &envCubemap);
			glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
			for (unsigned int i = 0; i < 6; ++i)
			{
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F,
					512, 512, 0, GL_RGB, GL_FLOAT, nullptr);
			}
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}

		static void ConvertEquiretangularToCubemap(Shader& conversionShader, GLuint& hdrTexture, GLuint& cubeViewFBO, u32 envCubemap, std::function<void()> renderCube) {
			mat4 cubeProjection = amath::perspective(amath::radians(90.0f), 1.0f, 0.1f, 10.0f);
			mat4 cubeViews[] =
			{
			   amath::lookAt(vec3(0.0f, 0.0f, 0.0f), vec3(1.0f,  0.0f,  0.0f), vec3(0.0f, -1.0f,  0.0f)),
			   amath::lookAt(vec3(0.0f, 0.0f, 0.0f), vec3(-1.0f,  0.0f,  0.0f), vec3(0.0f, -1.0f,  0.0f)),
			   amath::lookAt(vec3(0.0f, 0.0f, 0.0f), vec3(0.0f,  1.0f,  0.0f), vec3(0.0f,  0.0f,  1.0f)),
			   amath::lookAt(vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, -1.0f,  0.0f), vec3(0.0f,  0.0f, -1.0f)),
			   amath::lookAt(vec3(0.0f, 0.0f, 0.0f), vec3(0.0f,  0.0f,  1.0f), vec3(0.0f, -1.0f,  0.0f)),
			   amath::lookAt(vec3(0.0f, 0.0f, 0.0f), vec3(0.0f,  0.0f, -1.0f), vec3(0.0f, -1.0f,  0.0f))
			};
			glDepthFunc(GL_LEQUAL);
			conversionShader.use();
			conversionShader.setInt("equiretangularMap",0);
			conversionShader.setMat4x4("proj", cubeProjection);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, hdrTexture);

			glViewport(0,0,512,512);
			glBindFramebuffer(GL_FRAMEBUFFER, cubeViewFBO);
			for (u32 i = 0; i < 6; ++i) {
				conversionShader.setMat4x4("view", cubeViews[i]);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, envCubemap, 0);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			}
			renderCube();
			glBindFramebuffer(GL_FRAMEBUFFER,0);
			glDepthFunc(GL_LESS);
		}
	};
}