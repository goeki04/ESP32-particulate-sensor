#pragma once
#include "ISubsystem.h"
class Renderer : public ISubsystem{
	void start() override;
	void update() override;
};