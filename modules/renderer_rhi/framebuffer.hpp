#pragma once
#include "a_primitives.hpp"
namespace Andromeda {
	class framebufferManager {
	public:
		u32 m_SelectionDepth = 0;
		u32 m_Framebuffer = 0, m_MsaaFramebuffer = 0;
		u32 m_FramebufferTexture = 0, m_MsaaFramebufferTexture = 0;
		u32 m_SelectionFramebuffer = 0, m_SelectionTexture = 0;
		u32 m_PostprocessFramebuffer = 0, m_PostprocessTexture = 0;
		u32 m_Rendererbuffer = 0;
		void createSceneFbo(const ivec2 framebufferSize);
		void createMSAAFbo(const ivec2 framebufferSize,const u32 samples);
		void createSelectionFBO(const ivec2 framebufferSize);
		void createPostprocessFBO(const ivec2 framebufferSize);
		void createFramebuffers(const ivec2 framebufferSize, const u32 samples);
		void destroyFramebuffers();
	};
}