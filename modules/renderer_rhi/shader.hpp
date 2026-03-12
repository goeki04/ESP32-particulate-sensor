#pragma once
#include <unordered_map>
#include <string>
#include "a_material.hpp"
#include "a_math.hpp"
#include <GL/glew.h>
namespace Andromeda {
	class amath::CameraData;
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
		virtual ~Shader() {
			if (m_Program) glDeleteProgram(m_Program);
		}
		void setMat4x4(const char* uniformName, const mat4& matrix);
		void setVec3(const char* uniformName, const vec3& vector);
		void setVec2(const char* uniformName, const vec2& vector);
		void setFloat(const char* uniformName, const float floatVal);
		void setInt(const char* uniformName, const i32 intValue);
		void setTexture(const char* uniformName, u32 textureID, u32 slot);
		std::string readShaderSource(const char* shaderPath);
		void compileShader();
		void use() const {
			glUseProgram(m_Program);
		}
	protected:
		void setCameraUniforms(const amath::CameraData& cam);
	private:
		std::unordered_map<std::string, i32> m_UniformCache;
		i32 getUniformLocation(const char* name) {
			auto it = m_UniformCache.find(name);
			if (it != m_UniformCache.end())
				return it->second;

			i32 loc = glGetUniformLocation(m_Program, name);
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
		virtual void setUniforms(const amath::CameraData& cam, const mat4& modelMatrix) = 0;
	};

	class ProceduralShader : public Shader {
	public:
		ProceduralShader(const char* vertexPath, const char* fragmentPath) : Shader(vertexPath, fragmentPath) {
		}
		virtual ~ProceduralShader() = default;
		virtual void setUniforms(const amath::CameraData& cam) = 0;

	};

	class GridShader : public ProceduralShader {
	public:
		GridShader(const char* vertexPath, const char* fragmentPath) : ProceduralShader(vertexPath, fragmentPath) {}
		void setUniforms(const amath::CameraData& cam) override;
	};

	class LitShader : public MaterialShader {
	public:
		DirLight m_DirLight;
		vec3 m_AmbientLight;
		LitShader(const char* vertexPath, const char* fragmentPath) : MaterialShader(vertexPath, fragmentPath) {
			m_DirLight.color = vec3(1.0f, 1.0f, 1.0f);
			m_DirLight.direction = vec3(1.0f, 1.0f, 0.5f);
			m_AmbientLight = vec3(0.4f);
		}
		void setUniforms(const amath::CameraData& cam, const mat4& modelMatrix) override;
	};

	class ColorShader : public MaterialShader {
	public:
		vec3 color = vec3(1.0f, 1.0f, 1.0f);
		ColorShader(const char* vertexPath, const char* fragmentPath) : MaterialShader(vertexPath, fragmentPath) {}
		void setUniforms(const amath::CameraData& cam, const mat4& modelMatrix) override;
	};

	class OutlineShader : public PostProcessShader {
	public:
		OutlineShader(const char* vertexPath, const char* fragmentPath) : PostProcessShader(vertexPath, fragmentPath) {}
	};
}