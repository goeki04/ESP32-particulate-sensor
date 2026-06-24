#pragma once

/**
 * @file a_opengl_framebuffer.hpp
 * @brief OpenGL implementation of the render-target (framebuffer) interface.
 */

#include "a_rhi_framebuffer.hpp"
#include <GL/glew.h>
#include <vector>
#include "a_texture.hpp"

namespace Andromeda {

    /**
     * @class GLFramebuffer
     * @brief OpenGL framebuffer object (FBO) with its color and depth/stencil attachments, implementing @c IFramebuffer.
     */
    class GLFramebuffer : public IFramebuffer {
    public:
        /**
         * @brief Creates an OpenGL framebuffer from the given specification.
         * @param specs Size, sample count and attachment formats to build.
         */
        GLFramebuffer(const FramebufferSpecification& specs);

        /** @brief Destroys the framebuffer and its attachments. */
        virtual ~GLFramebuffer() override;

        /** @brief (Re)creates the FBO and all attachments from the current specification. */
        void invalidate();

        /** @brief Deletes the underlying OpenGL framebuffer and attachment objects. */
        void destroy();

        /** @copydoc IFramebuffer::getColorAttachmentTexture */
        const Texture& getColorAttachmentTexture(i32 index) const override;

        /** @copydoc IFramebuffer::resize */
        virtual void resize(const ivec2& newSize) override;

        /** @copydoc IFramebuffer::getSpecification */
        const FramebufferSpecification& getSpecification() const override;

        /** @brief Returns the raw OpenGL framebuffer object ID. */
        u32 getFramebufferID() const { return m_RendererID; }

    private:
        /**
         * @brief Creates and attaches the color textures described by the specification.
         * @param textureTarget The GL texture target (e.g. GL_TEXTURE_2D or the multisample target).
         * @param colorSpecs The color attachment specifications to create.
         */
        void attachColorAttachments(GLenum textureTarget, const std::vector<FramebufferTextureSpecification>& colorSpecs);

        /**
         * @brief Creates and attaches the depth (or depth/stencil) attachment if requested.
         * @param isMultisampled Whether to create a multisampled depth attachment.
         * @param depthSpec The depth attachment specification.
         * @param hasDepth Whether a depth attachment is required at all.
         */
        void attachDepthAttachment(bool isMultisampled, const FramebufferTextureSpecification& depthSpec, bool hasDepth);

        /** @brief Configures the GL draw-buffer list to match the color attachments. */
        void configureDrawBuffers();

        u32 m_RendererID = 0;                    ///< OpenGL framebuffer object ID.
        FramebufferSpecification m_Specs;        ///< Specification this framebuffer was created from.
        std::vector<Texture> m_ColorAttachments; ///< The created color attachment textures.
        u32 m_DepthAttachment = 0;               ///< OpenGL ID of the depth/stencil attachment (0 if none).
    };
}