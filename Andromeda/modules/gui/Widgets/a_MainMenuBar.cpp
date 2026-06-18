#include "a_MainMenuBar.hpp"
#include "a_EditorContext.hpp"
#include "gui_renderer.h"
#include "sceneSerializer.hpp"
#include "a_Docking.hpp"
#include <iostream>
extern std::string g_ProjectPath;

namespace Andromeda::Gui {
    void MainMenuBar::drawMainMenuBar(EditorContext& ctx)
    {
        ImGuiStyle& style = ImGui::GetStyle();
        style.FramePadding.y = 9.0f;
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBackground;
        if (ImGui::BeginMainMenuBar())
        {
            drawFileMenu(ctx);
            drawWindowMenu(ctx);
            drawDebugMenu();
            drawInfoMenu();
            ImGui::EndMainMenuBar();
            ImGui::PopStyleVar();
        }
    }
    void MainMenuBar::drawFileMenu(EditorContext& ctx)
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Save")) {
				std::cout << "project path:" <<  g_ProjectPath << std::endl;
                if (!SceneSerializer::save(g_ProjectPath + "\\scene.json", *ctx.registry)) {
                    throw std::runtime_error("saving the scene has failed!");
                }
            }
            if (ImGui::MenuItem("Load")) {
                if (!SceneSerializer::load(g_ProjectPath + "\\scene.json", *ctx.registry)) {
                    throw std::runtime_error("loading the scene has failed!");
                }
            }
            if (ImGui::MenuItem("Exit")) {}
            ImGui::EndMenu();
        }
    }
    void MainMenuBar::drawDebugMenu()
    {
        if (ImGui::BeginMenu("Debug")) {
            const float fps = ImGui::GetIO().Framerate;
            const float ms = 1000.0f / fps;
            
            ImGui::Text("Performance: %.1f FPS (%.2f ms/frame)", fps, ms);
            ImGui::Separator();

            ImGui::Text("Vendor: %s", glGetString(GL_VENDOR));
            ImGui::Text("Renderer: %s", glGetString(GL_RENDERER));
            ImGui::Text("OpenGL Version: %s", glGetString(GL_VERSION));
            ImGui::Text("GLSL Version: %s", glGetString(GL_SHADING_LANGUAGE_VERSION));
            ImGui::EndMenu();
        }
    }
    void MainMenuBar::drawInfoMenu()
    {
        if (ImGui::BeginMenu("Info"))
        {
            if (ImGui::MenuItem("Licenses")) {
                GuiRenderer::OpenFolder();
            }
            if (ImGui::MenuItem("SDK")) {
                GuiRenderer::OpenURL("https://www.bosch-sensortec.com/software-tools/software/previous-sdk-bmv-080-versions/");
            }
            if (ImGui::MenuItem("Github")) {
                GuiRenderer::OpenURL("https://github.com/goeki04/ESP32-particulate-sensor");
            }
            ImGui::EndMenu();
        }
    }
  
    void MainMenuBar::drawWindowMenu(EditorContext& ctx)
    {
        if (ImGui::BeginMenu("Window")) {
            if (ctx.panelController) {
                for (const auto& name : ctx.panelController->getPanelNames()) {
                    bool isOpen = ctx.panelController->isPanelOpen(name);
                    if (ImGui::MenuItem(name.data(), nullptr, &isOpen)) {
                        ctx.panelController->setPanelOpen(name, isOpen);
                    }
                }
            }
            else {
                ImGui::TextDisabled("No Panel Controller found!");
            }
            ImGui::EndMenu();
        }
    }
}