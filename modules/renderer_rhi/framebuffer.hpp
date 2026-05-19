#pragma once
#include "a_primitives.hpp"
namespace Andromeda {
	struct GLFramebuffer {
		u32 id = 0;
		u32 texture = 0;
		u32 depth = 0;

		void destroy();
	};
	class framebufferManager {
	public:
		GLFramebuffer m_Scene;
		GLFramebuffer m_Msaa;
		GLFramebuffer m_Selection;
		GLFramebuffer m_Postprocess;
		GLFramebuffer m_Baking;
		void createCubemapBakingFBO(u32 size);
		void createSceneFbo(ivec2 framebufferSize);
		void createMSAAFbo(ivec2 framebufferSize, u32 samples);
		void createSelectionFBO(ivec2 framebufferSize);
		void createPostprocessFBO(ivec2 framebufferSize);
		void createFramebuffers(ivec2 framebufferSize, u32 samples);
		void destroyFramebuffers();
	};
}