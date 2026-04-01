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
		void createSceneFbo(ivec2 framebufferSize);
		void createMSAAFbo(ivec2 framebufferSize, u32 samples);
		void createSelectionFBO(ivec2 framebufferSize);
		void createPostprocessFBO(ivec2 framebufferSize);
		void createFramebuffers(ivec2 framebufferSize, u32 samples);
		void destroyFramebuffers();
	};
}