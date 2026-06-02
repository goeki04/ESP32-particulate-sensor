#include "OpenGL/a_opengl_framebuffer.hpp"
#include <stdexcept>
#include <cassert>
#include "GL/glew.h"

namespace Andromeda {

    GLFramebuffer::GLFramebuffer(const FramebufferSpecification& specs)
        : m_Specs(specs)
    {
        invalidate();
    }

    GLFramebuffer::~GLFramebuffer() {
        destroy();
    }

    void GLFramebuffer::destroy() {
        if (m_RendererID) {
            glDeleteFramebuffers(1, &m_RendererID);

            if (!m_ColorAttachments.empty()) {
                std::vector<u32> textureIDs;
                textureIDs.reserve(m_ColorAttachments.size());

                for (const auto& tex : m_ColorAttachments) {
                    textureIDs.push_back(tex.textureID);
                }

                glDeleteTextures(static_cast<GLsizei>(textureIDs.size()), textureIDs.data());
            }

            if (m_DepthAttachment) {
                glDeleteTextures(1, &m_DepthAttachment);
                glDeleteRenderbuffers(1, &m_DepthAttachment);
            }

            m_ColorAttachments.clear();
            m_DepthAttachment = 0;
            m_RendererID = 0;
        }
    }

    const Texture& GLFramebuffer::getColorAttachmentTexture(i32 index) const
    {
        assert(index < m_ColorAttachments.size() && "Framebuffer attachment index out of bounds!");
        return m_ColorAttachments[index];
    }

    void GLFramebuffer::invalidate() {
        if (m_RendererID) {
            destroy();
        }

        glGenFramebuffers(1, &m_RendererID);
        glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);

        bool isMultisampled = m_Specs.samples > 1;
        GLenum textureTarget = isMultisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;

        std::vector<FramebufferTextureSpecification> colorSpecs;
        FramebufferTextureSpecification depthSpec;
        bool hasDepth = false;

        for (const auto& spec : m_Specs.attachments) {
            if (spec.textureFormat == FramebufferTextureFormat::DEPTH24Stencil8 || spec.textureFormat == FramebufferTextureFormat::DEPTH32F) {
                depthSpec = spec;
                hasDepth = true;
            }
            else {
                colorSpecs.push_back(spec);
            }
        }

        if (!colorSpecs.empty()) {
            m_ColorAttachments.resize(colorSpecs.size());

            for (size_t i = 0; i < colorSpecs.size(); ++i) {
                SamplerState sampler;
                sampler.type = isMultisampled ? TextureType::Cubemap : TextureType::Texture2D;
                sampler.minFilter = (colorSpecs[i].textureFormat == FramebufferTextureFormat::None) ? FilterModeMin::Nearest : FilterModeMin::Linear;
                sampler.magFilter = (colorSpecs[i].textureFormat == FramebufferTextureFormat::None) ? FilterModeMag::Nearest : FilterModeMag::Linear;
                sampler.wrapS = WrapMode::ClampToEdge;
                sampler.wrapT = WrapMode::ClampToEdge;

                m_ColorAttachments[i].width = m_Specs.width;
                m_ColorAttachments[i].height = m_Specs.height;
                m_ColorAttachments[i].sampler = sampler;

                glGenTextures(1, &m_ColorAttachments[i].textureID);
                glBindTexture(textureTarget, m_ColorAttachments[i].textureID);

                GLenum internalFormat = GL_RGBA8;
                GLenum format = GL_RGBA;
                GLenum dataType = GL_UNSIGNED_BYTE;

                if (colorSpecs[i].textureFormat == FramebufferTextureFormat::RGBA16F) {
                    internalFormat = GL_RGBA16F;
                    dataType = GL_FLOAT;
                }
                else if (colorSpecs[i].textureFormat == FramebufferTextureFormat::None) {
                    internalFormat = GL_R8;
                    format = GL_RED;
                }

                if (isMultisampled) {
                    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, m_Specs.samples, internalFormat, m_Specs.width, m_Specs.height, GL_TRUE);
                }
                else {
                    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_Specs.width, m_Specs.height, 0, format, dataType, nullptr);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, sampler.minFilter == FilterModeMin::Nearest ? GL_NEAREST : GL_LINEAR);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, sampler.magFilter == FilterModeMag::Nearest ? GL_NEAREST : GL_LINEAR);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                }

                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + static_cast<GLenum>(i), textureTarget, m_ColorAttachments[i].textureID, 0);
            }
        }

        if (hasDepth) {
            if (isMultisampled) {
                glGenRenderbuffers(1, &m_DepthAttachment);
                glBindRenderbuffer(GL_RENDERBUFFER, m_DepthAttachment);
                glRenderbufferStorageMultisample(GL_RENDERBUFFER, static_cast<GLsizei>(m_Specs.samples), GL_DEPTH24_STENCIL8, m_Specs.width, m_Specs.height);
                glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_DepthAttachment);
            }
            else {
                glGenTextures(1, &m_DepthAttachment);
                glBindTexture(GL_TEXTURE_2D, m_DepthAttachment);

                if (depthSpec.textureFormat == FramebufferTextureFormat::DEPTH32F) {
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, m_Specs.width, m_Specs.height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
                    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_DepthAttachment, 0);
                }
                else {
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, m_Specs.width, m_Specs.height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);
                    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_DepthAttachment, 0);
                }
            }
        }

        if (m_ColorAttachments.size() > 1) {
            GLenum buffers[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
            glDrawBuffers(static_cast<GLsizei>(m_ColorAttachments.size()), buffers);
        }
        else if (m_ColorAttachments.empty()) {
            glDrawBuffer(GL_NONE);
            glReadBuffer(GL_NONE);
        }

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            throw std::runtime_error("Andromeda RHI: Framebuffer is incomplete!");
        }

        glBindTexture(textureTarget, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void GLFramebuffer::resize(const ivec2& newSize) {
        if (newSize.x <= 0 || newSize.y <= 0) return;
        m_Specs.width = newSize.x;
        m_Specs.height = newSize.y;
        invalidate();
    }

    const FramebufferSpecification& GLFramebuffer::getSpecification() const {
        return m_Specs;
    }
}