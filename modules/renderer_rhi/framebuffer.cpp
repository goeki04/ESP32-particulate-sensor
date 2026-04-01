#include "framebuffer.hpp"
#include <GL/glew.h>
#include <stdexcept>
namespace Andromeda {
    void framebufferManager::createSceneFbo(const ivec2 framebufferSize)
    {
        //Color attachment & main fbo
        glGenFramebuffers(1, &m_Framebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, m_Framebuffer);
        glGenTextures(1, &m_FramebufferTexture);
        glBindTexture(GL_TEXTURE_2D, m_FramebufferTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, framebufferSize.x, framebufferSize.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_FramebufferTexture, 0);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            throw std::runtime_error("Framebuffer is not complete");
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    void framebufferManager::createMSAAFbo(const ivec2 framebufferSize, const u32 samples)
    {
        glGenFramebuffers(1, &m_MsaaFramebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, m_MsaaFramebuffer);
        glGenTextures(1, &m_MsaaFramebufferTexture);
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_MsaaFramebufferTexture);
        glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, static_cast<GLsizei>(samples), GL_RGB, framebufferSize.x, framebufferSize.y, GL_TRUE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, m_MsaaFramebufferTexture, 0);

        glGenRenderbuffers(1, &m_Rendererbuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, m_Rendererbuffer);
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, static_cast<GLsizei>(samples), GL_DEPTH24_STENCIL8, framebufferSize.x, framebufferSize.y);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_Rendererbuffer);
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            throw std::runtime_error("MSAA FBO incomplete!");
    }
    void framebufferManager::createSelectionFBO(const ivec2 framebufferSize)
    {
        //create selection framebuffer
        glGenFramebuffers(1, &m_SelectionFramebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, m_SelectionFramebuffer);

        glGenTextures(1, &m_SelectionTexture);
        glBindTexture(GL_TEXTURE_2D, m_SelectionTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, framebufferSize.x, framebufferSize.y, 0, GL_RED, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_SelectionTexture, 0);

        glGenRenderbuffers(1, &m_SelectionDepth);
        glBindRenderbuffer(GL_RENDERBUFFER, m_SelectionDepth);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, framebufferSize.x, framebufferSize.y);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_SelectionDepth);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            throw std::runtime_error("Selection Framebuffer incomplete!");

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    void framebufferManager::createPostprocessFBO(const ivec2 framebufferSize) {
        glGenFramebuffers(1, &m_PostprocessFramebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, m_PostprocessFramebuffer);

        glGenTextures(1, &m_PostprocessTexture);
        glBindTexture(GL_TEXTURE_2D, m_PostprocessTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, framebufferSize.x, framebufferSize.y,
            0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_PostprocessTexture, 0);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            throw std::runtime_error("Postprocess FBO incomplete!");

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void framebufferManager::createFramebuffers(const ivec2 framebufferSize, u32 samples)
    {
        createMSAAFbo(framebufferSize,samples);
        createSceneFbo(framebufferSize);
        createSelectionFBO(framebufferSize);
        createPostprocessFBO(framebufferSize);
    }

    void framebufferManager::destroyFramebuffers() {
        if (m_FramebufferTexture)
            glDeleteTextures(1, &m_FramebufferTexture);
        if (m_PostprocessTexture) {
            glDeleteTextures(1, &m_PostprocessTexture);
        }
        if (m_MsaaFramebufferTexture)
            glDeleteTextures(1, &m_MsaaFramebufferTexture);
        if (m_SelectionTexture) {
            glDeleteTextures(1, &m_SelectionTexture);
        }
        if (m_Rendererbuffer)
            glDeleteRenderbuffers(1, &m_Rendererbuffer);
        if (m_SelectionDepth) {
            glDeleteRenderbuffers(1, &m_SelectionDepth);
        }
        if (m_Framebuffer)
            glDeleteFramebuffers(1, &m_Framebuffer);
        if (m_MsaaFramebuffer)
            glDeleteFramebuffers(1, &m_MsaaFramebuffer);
        if (m_PostprocessFramebuffer) {
            glDeleteFramebuffers(1, &m_PostprocessFramebuffer);
        }
        if (m_SelectionFramebuffer) {
            glDeleteFramebuffers(1, &m_SelectionFramebuffer);
        }
        m_Framebuffer = 0;
        m_MsaaFramebuffer = 0;
        m_PostprocessFramebuffer = 0;
        m_SelectionFramebuffer = 0;
    }
}