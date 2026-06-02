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

        const Texture& getColorAttachmentTexture(i32 index) const override;

        virtual void resize(const ivec2& newSize) override;
        const FramebufferSpecification& getSpecification() const override;

        u32 getFramebufferID() const { return m_RendererID; }

    private:
        u32 m_RendererID = 0;
        FramebufferSpecification m_Specs;
        u32 m_DepthAttachment = 0;
    };
}