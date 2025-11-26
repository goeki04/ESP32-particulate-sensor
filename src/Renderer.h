#pragma once
#include "ISubsystem.h"
class Renderer : public ISubsystem{
	void start() override;
	void update() override;
public:
	static constexpr const char* glsl_version = "#version 150";
};