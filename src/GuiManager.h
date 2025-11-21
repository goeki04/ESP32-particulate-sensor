#pragma once
#include "ISubsystem.h"
#include "WindowManager.h"
#include "SubsystemManager.h"
class GuiManager : public ISubsystem{
    public:
    void start() override;
    void update() override;
    void destroy() override;
    private:
    WindowManager* m_WindowManager = SystemManager::getInstance().getSubsystem<WindowManager>();
};