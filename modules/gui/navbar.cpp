#include "panels.h"
#include "gui_renderer.h"
#include "console.h"
void Andromeda::Gui::Panels::drawNavBar(GuiRenderer& guiRenderer)
{
    ImGuiStyle& style = ImGui::GetStyle();
    style.FramePadding.y = 9.0f;
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBackground;
    if (ImGui::BeginMainMenuBar())
    {
        guiRenderer.m_MenuBarHeight = ImGui::GetWindowSize().y;
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Save")) {}
            if (ImGui::MenuItem("Load")) {}
            if (ImGui::MenuItem("Exit")) {}
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Guide")) {
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
                guiRenderer.OpenFolder();
            }
            if (ImGui::MenuItem("SDK")) {
                guiRenderer.OpenURL("https://www.bosch-sensortec.com/software-tools/software/previous-sdk-bmv-080-versions/");
            }
            if (ImGui::MenuItem("Github")) {
                guiRenderer.OpenURL("https://github.com/goeki04/ESP32-particulate-sensor");
            }
            if (ImGui::MenuItem("Version")) {
                guiRenderer.m_ShowVersion = true;
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Window")) {
            if (ImGui::MenuItem("Console", nullptr, guiRenderer.m_ConsoleOpen)) {
                guiRenderer.m_ConsoleOpen = !guiRenderer.m_ConsoleOpen;
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
        if (guiRenderer.m_ConsoleOpen) {
            static Console::AppConsole networkConsole;
            networkConsole.draw("Console", &guiRenderer.m_ConsoleOpen);
        }
        ImGui::PopStyleVar();
        if (guiRenderer.m_ShowVersion) {
            ImGui::SetNextWindowSize(ImVec2(guiRenderer.m_WidgetWidth, guiRenderer.m_WidgetWidth), ImGuiCond_FirstUseEver);
            ImGui::Begin("Version", &guiRenderer.m_ShowVersion, guiRenderer.m_WindowFlags);
            ImGui::TextUnformatted("App version: V1.0.0");
            std::string osName = "OS: " + std::string(ANDROMEDA_OS);
            ImGui::TextUnformatted(osName.c_str());
            std::string arch = "Arch: " + std::string(ANDROMEDA_ARCH);
            ImGui::TextUnformatted(arch.c_str());
            ImGui::TextUnformatted("Dev: Goekdeniz Koeksal");
            ImGui::TextUnformatted(guiRenderer.m_ImguiVersion.c_str());

            int major = SDL_MAJOR_VERSION;
            int minor = SDL_MINOR_VERSION;
            int patch = SDL_MICRO_VERSION;
            std::string sdlVersion = std::format("SDL {}.{}.{}", major, minor, patch);
            ImGui::TextUnformatted(sdlVersion.c_str());
            ImGui::End();
        }
    }
}