#pragma once
#include "camera.h"
#include <unordered_map>
#include "a_material.hpp"
#include <GL/glew.h>
namespace Andromeda {
	class Shader {
	public:
		constexpr static const char* c_viewMatrix = "viewMatrix";
		constexpr static const char* c_projMatrix = "projMatrix";
		constexpr static const char* c_camPos = "camPos";
		GLuint m_Program = 0;
		std::string m_VertexShaderPath;
		std::string m_FragmentShaderPath;
		Shader(const char* vertexPath, const char* fragmentPath) :
			m_VertexShaderPath(vertexPath), m_FragmentShaderPath(fragmentPath) {
		}
		Shader(const Shader&) = delete;
		Shader& operator=(const Shader&) = delete;
		virtual ~Shader() = default;
		void setMat4x4(const char* uniformName, const glm::mat4& matrix);
		void setVec3(const char* uniformName, const glm::vec3& vector);
		void setVec2(const char* uniformName, const glm::vec2& vector);
		void setFloat(const char* uniformName, const float floatVal);
		void setInt(const char* uniformName, const int intValue);
		void setTexture(const char* uniformName, GLuint textureID, GLuint slot);
		std::string readShaderSource(const char* shaderPath);
		void compileShader();
		void use() const {
			glUseProgram(m_Program);
		}
	protected:
		void setCameraUniforms(const Camera& cam) {
			setMat4x4(c_viewMatrix, cam.getViewMatrix());
			setMat4x4(c_projMatrix, cam.getProjectionMatrix());
			setVec3(c_camPos, cam.getCameraPos());
		}
	private:
		std::unordered_map<std::string, GLint> m_UniformCache;
		GLint getUniformLocation(const char* name) {
			auto it = m_UniformCache.find(name);
			if (it != m_UniformCache.end())
				return it->second;

			GLint loc = glGetUniformLocation(m_Program, name);
			m_UniformCache[name] = loc;
			return loc;
		}
	};

	class PostProcessShader : public Shader {
	public:
		PostProcessShader(const char* vertexPath, const char* fragmentPath)
			: Shader(vertexPath, fragmentPath) {
		};
		virtual ~PostProcessShader() = default;
	};

	class MaterialShader : public Shader {
	public:
		MaterialShader(const char* vertexPath, const char* fragmentPath) : Shader(vertexPath, fragmentPath) {
		}
		virtual ~MaterialShader() = default;
		virtual void setUniforms(const Camera& cam, const glm::mat4& modelMatrix) = 0;
	};

	class ProceduralShader : public Shader {
	public:
		ProceduralShader(const char* vertexPath, const char* fragmentPath) : Shader(vertexPath, fragmentPath) {
		}
		virtual ~ProceduralShader() = default;
		virtual void setUniforms(const Camera& cam) = 0;

	};

	class GridShader : public ProceduralShader {
	public:
		GridShader(const char* vertexPath, const char* fragmentPath) : ProceduralShader(vertexPath, fragmentPath) {}
		void setUniforms(const Camera& cam) override;
	};

	class LitShader : public MaterialShader {
	public:
		DirLight m_DirLight;
		glm::vec3 m_AmbientLight;
		LitShader(const char* vertexPath, const char* fragmentPath) : MaterialShader(vertexPath, fragmentPath) {
			m_DirLight.color = glm::vec3(1.0f, 1.0f, 1.0f);
			m_DirLight.direction = glm::vec3(1.0f, 1.0f, 0.5f);
			m_AmbientLight = glm::vec3(0.4f);
		}
		void setUniforms(const Camera& cam, const glm::mat4& modelMatrix) override;
	};

	class ColorShader : public MaterialShader {
	public:
		glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
		ColorShader(const char* vertexPath, const char* fragmentPath) : MaterialShader(vertexPath, fragmentPath) {}
		void setUniforms(const Camera& cam, const glm::mat4& modelMatrix) override;
	};

	class OutlineShader : public PostProcessShader {
	public:
		OutlineShader(const char* vertexPath, const char* fragmentPath) : PostProcessShader(vertexPath, fragmentPath) {}
	};
}