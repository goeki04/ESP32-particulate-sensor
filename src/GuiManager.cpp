#include "GuiManager.h"
#include "pch.h"
#include "SubsystemManager.h"
void GuiManager::start(){
    m_WindowManager = SystemManager::getInstance().getSubsystem<WindowManager>();
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    ImGui::StyleColorsDark();
    ImGuiStyle& style = ImGui::GetStyle();
    float mainScale = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());
    style.ScaleAllSizes(mainScale);        // Bake a fixed style scale. (until we have a solution for dynamic style scaling, changing this requires resetting Style + calling this again)
    style.FontScaleDpi = mainScale;

    // Setup Platform/Renderer backends
    ImGui_ImplSDL3_InitForSDLRenderer(m_WindowManager->getWindow(), m_WindowManager->getRenderer());
    ImGui_ImplSDLRenderer3_Init(m_WindowManager->getRenderer());
}

void GuiManager::update(){

}

void GuiManager::updateEvent(SDL_Event* event)
{
    ImGui_ImplSDL3_ProcessEvent(event);
}

void GuiManager::destroy(){
    ImGui_ImplSDLRenderer3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();
}