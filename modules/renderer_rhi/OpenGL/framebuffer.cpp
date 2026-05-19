#include "framebuffer.hpp"
#include <GL/glew.h>
#include <stdexcept>

namespace Andromeda {

    void framebufferManager::createCubemapBakingFBO(u32 size)
    {
        glGenFramebuffers(1, &m_Baking.id);
        glGenRenderbuffers(1, &m_Baking.depth);

        glBindFramebuffer(GL_FRAMEBUFFER, m_Baking.id);
        glBindRenderbuffer(GL_RENDERBUFFER, m_Baking.depth);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, size, size);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_Baking.depth);
    }

    void framebufferManager::createSceneFbo(const ivec2 size)
    {
        glGenFramebuffers(1, &m_Scene.id);
        glBindFramebuffer(GL_FRAMEBUFFER, m_Scene.id);

        glGenTextures(1, &m_Scene.texture);
        glBindTexture(GL_TEXTURE_2D, m_Scene.texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size.x, size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_Scene.texture, 0);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            throw std::runtime_error("Scene FBO incomplete!");

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void framebufferManager::createMSAAFbo(const ivec2 size, const u32 samples)
    {
        glGenFramebuffers(1, &m_Msaa.id);
        glBindFramebuffer(GL_FRAMEBUFFER, m_Msaa.id);

        glGenTextures(1, &m_Msaa.texture);
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_Msaa.texture);
        glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, static_cast<GLsizei>(samples), GL_RGB, size.x, size.y, GL_TRUE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, m_Msaa.texture, 0);

        glGenRenderbuffers(1, &m_Msaa.depth);
        glBindRenderbuffer(GL_RENDERBUFFER, m_Msaa.depth);
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, static_cast<GLsizei>(samples), GL_DEPTH24_STENCIL8, size.x, size.y);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_Msaa.depth);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            throw std::runtime_error("MSAA FBO incomplete!");

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void framebufferManager::createSelectionFBO(const ivec2 size)
    {
        glGenFramebuffers(1, &m_Selection.id);
        glBindFramebuffer(GL_FRAMEBUFFER, m_Selection.id);

        glGenTextures(1, &m_Selection.texture);
        glBindTexture(GL_TEXTURE_2D, m_Selection.texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, size.x, size.y, 0, GL_RED, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_Selection.texture, 0);

        glGenRenderbuffers(1, &m_Selection.depth);
        glBindRenderbuffer(GL_RENDERBUFFER, m_Selection.depth);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, size.x, size.y);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_Selection.depth);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            throw std::runtime_error("Selection Framebuffer incomplete!");

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void framebufferManager::createPostprocessFBO(const ivec2 size)
    {
        glGenFramebuffers(1, &m_Postprocess.id);
        glBindFramebuffer(GL_FRAMEBUFFER, m_Postprocess.id);

        glGenTextures(1, &m_Postprocess.texture);
        glBindTexture(GL_TEXTURE_2D, m_Postprocess.texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size.x, size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_Postprocess.texture, 0);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            throw std::runtime_error("Postprocess FBO incomplete!");

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void framebufferManager::createFramebuffers(const ivec2 size, u32 samples)
    {
        createMSAAFbo(size, samples);
        createSceneFbo(size);
        createSelectionFBO(size);
        createPostprocessFBO(size);
    }

    void framebufferManager::destroyFramebuffers()
    {
        m_Scene.destroy();
        m_Msaa.destroy();
        m_Selection.destroy();
        m_Postprocess.destroy();
        m_Baking.destroy();
    }
    void GLFramebuffer::destroy()
    {
        if (id) {
            glDeleteFramebuffers(1, &id);
            id = 0;
        }
        if (texture) {
            glDeleteTextures(1, &texture);
            texture = 0;
        }
        if (depth) {
            glDeleteRenderbuffers(1, &depth);
            depth = 0;
        }
    }
}