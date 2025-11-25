#include "GuiManager.h"
#include "pch.h"
#include "SubsystemManager.h"
void GuiManager::init(SDL_Window* window, SDL_Renderer* renderer){
    IMGUI_CHECKVERSION();
    ImGui::CreateContext(); 
    ImGuiIO& io = ImGui::GetIO();(void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    ImGui::StyleColorsDark();
    ImGuiStyle& style = ImGui::GetStyle();
    float mainScale = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());
    style.ScaleAllSizes(mainScale);        // Bake a fixed style scale. (until we have a solution for dynamic style scaling, changing this requires resetting Style + calling this again)
    style.FontScaleDpi = mainScale;
    // Setup Platform/Renderer backends
    ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer3_Init(renderer);
    SDL_GetWindowSizeInPixels(window,&m_WindowWidth,&m_WindowHeight);
    m_WindowFlags |= ImGuiWindowFlags_NoCollapse;
    m_WindowFlags |= ImGuiWindowFlags_NoResize;
}

void GuiManager::draw() {
    ImGuiIO& io = ImGui::GetIO();(void)io;
    // Start the Dear ImGui frame
    ImGui_ImplSDLRenderer3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();
    drawNavBar();
    drawNotification();
    //drawInformation();
}

void GuiManager::destroy(){
    ImGui_ImplSDLRenderer3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();
}

void GuiManager::drawNavBar()
{
    ImGuiStyle& style = ImGui::GetStyle();
    style.FramePadding.y = 9.0f; // dickere Buttons
    style.WindowBorderSize = 0.0f;
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBackground;
    if (ImGui::BeginMainMenuBar())
    {
        m_MenuBarHeight = ImGui::GetWindowSize().y;
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Save")) {  }
            if (ImGui::MenuItem("Load")) {  }
            if (ImGui::MenuItem("Exit")) {  }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Options"))
        {
            if (ImGui::MenuItem("Language")) {}
            if (ImGui::MenuItem("Window")) {}
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Help"))
        {
            if (ImGui::MenuItem("Licenses")) {  }
            if (ImGui::MenuItem("SDK's")) {  }
            if (ImGui::MenuItem("Github")) {}
            if (ImGui::MenuItem("Version")) {}
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}

void GuiManager::drawNotification()
{
    //Place top right
    ImVec2 widgetDimensions = ImVec2(m_WindowWidth * 0.125f, m_WindowHeight - m_MenuBarHeight);
    int windowPosX = m_WindowWidth - widgetDimensions.x;
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.172f, 0.172f, 0.329f,1.0f)); // RGBA
    ImGui::SetNextWindowPos(ImVec2(windowPosX,m_MenuBarHeight));
    ImGui::SetNextWindowSize(widgetDimensions);
    ImGui::Begin("Notifications", 0, m_WindowFlags);
    ImGui::End();
    ImGui::PopStyleColor();
}

void GuiManager::drawInformation()
{
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.172f, 0.172f, 0.329f, 1.0f));
    ImGui::SetNextWindowPos(ImVec2(0,0));
    ImGui::SetNextWindowSize(ImVec2(m_WindowWidth * 0.125f, m_WindowHeight * 0.5f));
    ImGui::Begin("Information", 0, m_WindowFlags);
    ImGui::End();
    ImGui::PopStyleColor();
}

