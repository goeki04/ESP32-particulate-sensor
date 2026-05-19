#pragma once
#include <unordered_map>
#include <string>
#include "a_material.hpp"
#include "a_math.hpp"
#include <GL/glew.h>
namespace Andromeda {
	/**
	 * @brief Base class for GLSL Shader Programs.
	 * Manages shader compilation, linking, and provides an optimized interface
	 * for updating GPU uniforms.
	 */
	class Shader {
	public:
		// Standard uniform names used across the engine
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
		// Shaders manage GPU resources; prevent accidental copies to avoid double-free
		virtual ~Shader() {
			if (m_Program) glDeleteProgram(m_Program);
		}
		// --- Uniform Upload Helpers ---
		void setMat4x4(const char* uniformName, const mat4& matrix);
		void setVec3(const char* uniformName, const vec3& vector);
		void setVec2(const char* uniformName, const vec2& vector);
		void setFloat(const char* uniformName,float floatVal);
		void setInt(const char* uniformName, i32 intValue);
		void setTexture(const char* uniformName, u32 textureID, u32 slot);
		/**
		 * @brief Reads shader source code from the provided file path.
		 */
		static std::string readShaderSource(const char* shaderPath);
		/**
		 * @brief Compiles vertex/fragment stages and links them into the final GPU program.
		 */
		void compileShader();
		/**
		 * @brief Binds the shader program for current rendering operations.
		 */
		void use() const {
			glUseProgram(m_Program);
		}
	protected:
		/**
		 * @brief Internal helper to upload standard camera matrices to the GPU.
		 */
		void setCameraUniforms(const amath::CameraData* cam);
	private:
		/**
		 * @brief Local cache to store uniform locations.
		 * Minimizes expensive calls to the OpenGL driver (glGetUniformLocation).
		 */
		std::unordered_map<std::string, i32> m_UniformCache;
		/**
		 * @brief Returns the location of a uniform, fetching it from the cache if possible.
		 */
		i32 getUniformLocation(const char* name) {
			auto it = m_UniformCache.find(name);
			if (it != m_UniformCache.end())
				return it->second;

			i32 loc = glGetUniformLocation(m_Program, name);
			m_UniformCache[name] = loc;
			return loc;
		}
	};
	/**
	 * @brief Specialized shader type for full-screen post-processing effects.
	 */
	class PostProcessShader : public Shader {
	public:
		PostProcessShader(const char* vertexPath, const char* fragmentPath)
			: Shader(vertexPath, fragmentPath) {
		};

		~PostProcessShader() override = default;
	};
	/**
	 * @brief Specialized shader for 3D geometry that requires a model matrix.
	 */
	class MaterialShader : public Shader {
	public:
		MaterialShader(const char* vertexPath, const char* fragmentPath) : Shader(vertexPath, fragmentPath) {
		}

		~MaterialShader() override = default;
		virtual void setUniforms(const amath::CameraData* cam, const mat4& modelMatrix) = 0;
	};

	/**
	 * @brief Shader for procedurally generated geometry or background effects.
	 */

	class ProceduralShader : public Shader {
	public:
		ProceduralShader(const char* vertexPath, const char* fragmentPath) : Shader(vertexPath, fragmentPath) {
		}

		~ProceduralShader() override = default;
		virtual void setUniforms(const amath::CameraData* cam) = 0;

	};
	// --- Concrete Shader Implementations ---

	/**
	 * @brief Shader specifically designed for rendering the infinite editor grid.
	 */
	class GridShader : public ProceduralShader {
	public:
		GridShader(const char* vertexPath, const char* fragmentPath) : ProceduralShader(vertexPath, fragmentPath) {}
		void setUniforms(const amath::CameraData* cam) override;
	};
	/**
	 * @brief Standard lighting shader supporting directional and ambient light.
	 */
	class LitShader : public MaterialShader {
	public:
		DirLight m_DirLight{};
		vec3 m_AmbientLight{};
		LitShader(const char* vertexPath, const char* fragmentPath) : MaterialShader(vertexPath, fragmentPath) {
			m_DirLight.color = vec3(1.0f, 1.0f, 1.0f);
			m_DirLight.direction = vec3(1.0f, 1.0f, 0.5f);
			m_AmbientLight = vec3(0.4f);
		}
		void setUniforms(const amath::CameraData* cam, const mat4& modelMatrix) override;
	};
	/**
	 * @brief Simple shader that renders objects with a uniform solid color.
	 */
	class ColorShader : public MaterialShader {
	public:
		vec3 color = vec3(1.0f, 1.0f, 1.0f);
		ColorShader(const char* vertexPath, const char* fragmentPath) : MaterialShader(vertexPath, fragmentPath) {}
		void setUniforms(const amath::CameraData* cam, const mat4& modelMatrix) override;
	};

	class CubemapTestShader : public MaterialShader {
	public:
		CubemapTestShader(const char* vertexPath, const char* fragmentPath) : MaterialShader(vertexPath, fragmentPath) {
		}

		void setUniforms(const amath::CameraData* cam, const mat4& modelMatrix) override {
			setCameraUniforms(cam);
			setMat4x4("model", modelMatrix);
		}
	};

	/**
	 * @brief Post-process effect shader used for highlighting selected entities.
	 */
	class OutlineShader : public PostProcessShader {
	public:
		OutlineShader(const char* vertexPath, const char* fragmentPath) : PostProcessShader(vertexPath, fragmentPath) {}
	};
}