#pragma once

#include "a_rhi_framebuffer.hpp"
#include <GL/glew.h>
#include <vector>

namespace Andromeda {

    class GLFramebuffer : public IFramebuffer {
    public:
        GLFramebuffer(const FramebufferSpecification& specs);
        virtual ~GLFramebuffer() override;

        void invalidate();
        void destroy();

        virtual void resize(const ivec2& newSize) override;
        virtual u32 getColorAttachmentRendererID(i32 index = 0) const override { return m_ColorAttachments[index]; }
        virtual const FramebufferSpecification& getSpecification() const override { return m_Specs; }

        u32 getFramebufferID() const { return m_RendererID; }

    private:
        u32 m_RendererID = 0;
        FramebufferSpecification m_Specs;

        std::vector<u32> m_ColorAttachments;
        u32 m_DepthAttachment = 0;
    };
}