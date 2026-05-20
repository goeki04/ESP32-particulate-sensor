#include "OpenGL/a_opengl_framebuffer.hpp"
#include <stdexcept>
#include <cassert>

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
                glDeleteTextures(static_cast<GLsizei>(m_ColorAttachments.size()), m_ColorAttachments.data());
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
            glGenTextures(static_cast<GLsizei>(colorSpecs.size()), m_ColorAttachments.data());

            for (size_t i = 0; i < colorSpecs.size(); ++i) {
                glBindTexture(textureTarget, m_ColorAttachments[i]);

                GLenum internalFormat = GL_RGBA8;
                GLenum format = GL_RGBA;
                GLenum filter = GL_LINEAR;

                if (colorSpecs[i].textureFormat == FramebufferTextureFormat::RGBA16F) {
                    internalFormat = GL_RGBA16F;
                }
                else if (colorSpecs[i].textureFormat == FramebufferTextureFormat::None) {
                    internalFormat = GL_R8;
                    format = GL_RED;
                    filter = GL_NEAREST;
                }

                if (isMultisampled) {
                    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, m_Specs.samples, internalFormat, m_Specs.width, m_Specs.height, GL_TRUE);
                }
                else {
                    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_Specs.width, m_Specs.height, 0, format, GL_UNSIGNED_BYTE, nullptr);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
                }

                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + static_cast<GLenum>(i), textureTarget, m_ColorAttachments[i], 0);
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

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void GLFramebuffer::resize(const ivec2& newSize) {
        if (newSize.x <= 0 || newSize.y <= 0) return;
        m_Specs.width = newSize.x;
        m_Specs.height = newSize.y;
        invalidate();
    }
}