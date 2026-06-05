#pragma once
#include <string>
#include "a_Primitives.hpp"
#include "a_IGraphicsContext.hpp"
namespace Andromeda::RendererRHI {

	class Shader
	{
    public:
        ShaderProgramHandle m_Handle;
        std::string m_VertexPath;
        std::string m_FragmentPath;

        Shader(const char* vertexPath, const char* fragmentPath, IGraphicsContext* context)
            : m_VertexPath(vertexPath), m_FragmentPath(fragmentPath), m_Context(context) {
        }

        ~Shader() {
            if (m_Handle.apiID != 0 && m_Context) {
                m_Context->destroyShaderProgram(m_Handle);
            }
        }

        Shader(const Shader&) = delete;
        Shader& operator=(const Shader&) = delete;

    private:
        IGraphicsContext* m_Context = nullptr;
    };
}