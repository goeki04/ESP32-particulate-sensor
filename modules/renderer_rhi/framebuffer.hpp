#pragma once
namespace Andromeda {
	class framebuffer {
		void createSceneFbo();
		void createMSAAFbo();
		void createSelectionFBO();
		void createPostprocessFBO();
		void createFramebuffers();
		void destroyFramebuffers();
	};
}