#pragma once

#include <unordered_map>
#include <string>
#include "a_math.hpp"
#include <GL/glew.h>

namespace Andromeda {

    /**
     * @brief Base class for GLSL Shader Programs (Legacy & Baking Support).
     * Manages shader compilation, linking, and providing low-level uniform access.
     */
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
        void setFloat(const char* uniformName, float floatVal);
        void setInt(const char* uniformName, i32 intValue);
        void setTexture(const char* uniformName, u32 textureID, u32 slot);

        static std::string readShaderSource(const char* shaderPath);
        void compileShader();

        void use() const {
            glUseProgram(m_Program);
        }

    protected:
        void setCameraUniforms(const amath::CameraData* cam);

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

    class BakingShader : public Shader {
    public:
        BakingShader(const char* vertexPath, const char* fragmentPath)
            : Shader(vertexPath, fragmentPath) {
        }

        ~BakingShader() override = default;
        virtual void setUniforms() = 0;
    };

    class EquirectangularShader : public BakingShader {
    public:
        mat4 proj = mat4(1);
        mat4 view = mat4(1);

        EquirectangularShader(const char* vertexPath, const char* fragmentPath)
            : BakingShader(vertexPath, fragmentPath) {
        }

        void setUniforms() override {
            setMat4x4("proj", proj);
            setMat4x4("view", view);
        }
    };
}