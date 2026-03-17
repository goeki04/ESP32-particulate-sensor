#include <gtest/gtest.h>

#include "subsystem_manager.h"
#include "window_manager.h"
#include "resource_manager.h"
#include "renderer.h"

using namespace Andromeda;

class SubsystemStartTest : public ::testing::Test {
protected:
    void SetUp() override {
        std::cout << "[ INFO ] Starte Test-Setup..." << std::endl;
    }
};

TEST_F(SubsystemStartTest, CheckWindowManager) {
    std::cout << "[ STEP ] Initialisiere WindowManager..." << std::endl;
    Window::WindowManager windowManager;

    ASSERT_NO_THROW({windowManager.start();}) << "Fehler im WindowManager::start()!";

    std::cout << "[ OK ] WindowManager erfolgreich gestartet." << std::endl;
}

TEST_F(SubsystemStartTest, CheckResourceManager) {
    std::cout << "[ STEP ] Initialisiere ResourceManager..." << std::endl;
    Andromeda::ResourceManager resourceManager;

    ASSERT_NO_THROW({resourceManager.start();}) << "Fehler im ResourceManager::start()!";

    std::cout << "[ OK ] ResourceManager erfolgreich gestartet." << std::endl;
}

TEST_F(SubsystemStartTest, CheckRenderer) {
    std::cout << "[ STEP ] Initialisiere Renderer..." << std::endl;
    Andromeda::Renderer renderer;

    ASSERT_NO_THROW({renderer.start(); }) << "Fehler im Renderer::start()!";

    std::cout << "[ OK ] Renderer erfolgreich gestartet." << std::endl;
}