#pragma once
#include "subsystem.h"
namespace Andromeda {
	class Editor : public ISubsystem{
		Renderer* m_Renderer = nullptr;
		void syncLibraryWithRM();
		void start() override;
		void update() override;
	};
}