
#include "gui_renderer.h"
#include "panels.h"
#include <stdexcept>
#include <algorithm>
#include "window_manager.hpp"
#include <filesystem>
#include "HierarchyPanel.hpp"
#include "../editor/editor.hpp"
#include <iostream>
namespace Andromeda::Gui {

    void Gui::GuiRenderer::init(EditorContext& editorContext) {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui_ImplSDL3_InitForOpenGL(editorContext.windowContext.window, editorContext.windowContext.glContext);
        ImGui_ImplOpenGL3_Init(editorContext.windowContext.glslVersion);
        ImGui::StyleColorsDark();
        setFlags();
        setStyle();
        loadFont();

        m_Panels.push_back(std::make_unique<HierarchyPanel>("Hierarchy"));
    }
    void GuiRenderer::update(const ViewportDrawInfo& vpInfo, EditorContext& editorContext) {
        
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();
        constexpr ImGuiDockNodeFlags dockFlags = ImGuiDockNodeFlags_PassthruCentralNode;
        ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport(), dockFlags);
        Panels::drawNavBar(editorContext);
        Panels::drawDeviceBrowser(editorContext);
        Panels::drawDetails(editorContext);
        for (auto& panel : m_Panels)
        {
            if (panel->m_IsOpen)
            {
                panel->onImGuiRender(editorContext);
            }
        }
        drawChart();
        Panels::drawViewportGUI(editorContext,*this, vpInfo);
        EndFrame(Window::g_Window);
    }

    void GuiRenderer::EndFrame(SDL_Window* window) {
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        const ImGuiIO& io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            SDL_Window* backup_current_window = SDL_GL_GetCurrentWindow();
            const SDL_GLContext backup_current_context = SDL_GL_GetCurrentContext();

            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();

            SDL_GL_MakeCurrent(backup_current_window, backup_current_context);
        }
        SDL_GL_SwapWindow(window);
    }

    void GuiRenderer::loadFont()
    {
        ImGuiIO& io = ImGui::GetIO();(void)io;
        ImFont* font = io.Fonts->AddFontFromFileTTF(ASSET_PATH "fonts/Roboto-Regular.ttf", 18.0f);
        if (font == nullptr) {
            throw std::runtime_error("failed loading Roboto font");
        }
        io.FontDefault = font;
    }
    void GuiRenderer::OpenFolder()
    {
#if defined(_WIN32)
        const auto targetDirectory = std::filesystem::current_path().parent_path() / "Licenses";
        const std::string cmd = "explorer \"" + targetDirectory.string() + "\"";
        system(cmd.c_str());
#endif
    }

    void GuiRenderer::drawChart() const
    {
        ImGui::Begin("Chart", nullptr);
        ImGui::End();
    }
    void GuiRenderer::OpenURL(const std::string& url)
    {
        SDL_OpenURL(url.c_str());
    }

    // ReSharper disable once CppMemberFunctionMayBeStatic
    void GuiRenderer::destroy() {
        ImGui::DestroyPlatformWindows();
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplSDL3_Shutdown();
        if (ImGui::GetCurrentContext()) {
            ImGui::DestroyContext();
        }
    }
    void GuiRenderer::setFlags()
    {
        ImGuiIO& io = ImGui::GetIO();(void)io;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    }
    /// <summary>
    /// color scheme
    /// </summary>
    void GuiRenderer::setStyle() {
        ImGuiStyle& style = ImGui::GetStyle();

        float mainScale = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());
        style.ScaleAllSizes(mainScale);
        style.WindowRounding = 2.0f;
        style.ChildRounding = 2.0f;
        style.FrameRounding = 2.0f;
        style.PopupRounding = 2.0f;
        style.ScrollbarRounding = 2.0f;
        style.TabRounding = 2.0f;
        style.WindowBorderSize = 1.0f;
        style.FrameBorderSize = 1.0f;
        style.ItemSpacing = ImVec2(8.0f, 4.0f);
        style.FramePadding = ImVec2(6.0f, 4.0f);

        ImVec4* colors = style.Colors;
        colors[ImGuiCol_WindowBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
        colors[ImGuiCol_ChildBg] = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);
        colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);

        colors[ImGuiCol_Border] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
        colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);

        colors[ImGuiCol_MenuBarBg] = ImVec4(0.08f, 0.08f, 0.08f, 1.00f);
        colors[ImGuiCol_TitleBg] = ImVec4(0.08f, 0.08f, 0.08f, 1.00f);
        colors[ImGuiCol_TitleBgActive] = ImVec4(0.08f, 0.08f, 0.08f, 1.00f);
        colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.08f, 0.08f, 0.08f, 1.00f);

        colors[ImGuiCol_FrameBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
        colors[ImGuiCol_FrameBgHovered] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
        colors[ImGuiCol_FrameBgActive] = ImVec4(0.28f, 0.28f, 0.28f, 1.00f);

        colors[ImGuiCol_Tab] = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);
        colors[ImGuiCol_TabHovered] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
        colors[ImGuiCol_TabActive] = ImVec4(0.18f, 0.18f, 0.18f, 1.00f);
        colors[ImGuiCol_TabUnfocused] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
        colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);

        colors[ImGuiCol_Button] = ImVec4(0.18f, 0.18f, 0.18f, 1.00f);
        colors[ImGuiCol_ButtonHovered] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
        colors[ImGuiCol_ButtonActive] = ImVec4(0.00f, 0.45f, 0.84f, 1.00f);

        colors[ImGuiCol_Header] = ImVec4(0.18f, 0.18f, 0.18f, 1.00f);
        colors[ImGuiCol_HeaderHovered] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
        colors[ImGuiCol_HeaderActive] = ImVec4(0.00f, 0.45f, 0.84f, 1.00f);

        colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
        colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
        colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.35f, 0.35f, 0.35f, 1.00f);
        colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.45f, 0.45f, 0.45f, 1.00f);

        colors[ImGuiCol_CheckMark] = ImVec4(0.00f, 0.45f, 0.84f, 1.00f);
        colors[ImGuiCol_SliderGrab] = ImVec4(0.00f, 0.45f, 0.84f, 1.00f);
        colors[ImGuiCol_SliderGrabActive] = ImVec4(0.00f, 0.55f, 0.95f, 1.00f);

        colors[ImGuiCol_Text] = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
        colors[ImGuiCol_TextDisabled] = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);
    }
}
