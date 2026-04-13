#include "panels.h"
#include "gui_renderer.h"
#include "console.h"
#include "sceneSerializer.hpp"
void Andromeda::Gui::Panels::drawNavBar(EditorContext& ctx)
{
    ImGuiStyle& style = ImGui::GetStyle();
    style.FramePadding.y = 9.0f;
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBackground;
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Save")) {
                if (!SceneSerializer::save("save_1.json", *ctx.registry)) {
                    throw std::runtime_error("saving the scene has failed!");
                }
            }
            if (ImGui::MenuItem("Load")) {
                if (!SceneSerializer::load("save_1.json", *ctx.registry)) {
                    throw std::runtime_error("loading the scene has failed!");
                }
            }
            if (ImGui::MenuItem("Exit")) {}
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Documentation")) {
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Options"))
        {
            if (ImGui::MenuItem("Language")) {}
            if (ImGui::MenuItem("Window")) {}
            ImGui::EndMenu();
        }

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
        if (ImGui::BeginMenu("Window")) {
            if (ImGui::MenuItem("Console", nullptr, ctx.state.consoleOpen)) {
                ctx.state.consoleOpen = !ctx.state.consoleOpen;
            }
            ImGui::EndMenu();
        }

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
        ImGui::EndMainMenuBar();

        ImGui::PopStyleVar();
        if (ctx.state.consoleOpen) {
            static Console::AppConsole networkConsole;
            networkConsole.draw("Console", &ctx.state.consoleOpen);
        }
    }
}