#include "gui_renderer.h"
#include <stdexcept>
#include "imgui_internal.h"
#include "window_manager.hpp"
#include <filesystem>
#include "a_MainMenuBar.hpp"
#include "a_HierarchyPanel.hpp"
#include "a_ConsolePanel.hpp"
#include "a_DetailsPanel.hpp"
#include "a_BrowserPanel.hpp"
#include "a_ViewportPanel.hpp"
#include "a_Style.hpp"
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
        m_Panels.push_back(std::make_unique<ConsolePanel>("Console"));
        m_Panels.push_back(std::make_unique<DetailsPanel>("Details"));
        m_Panels.push_back(std::make_unique<BrowserPanel>("Browser"));
        m_Panels.push_back(std::make_unique<ViewportPanel>("Viewport"));
    }

    void GuiRenderer::update(EditorContext& editorContext) {
        
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();
        constexpr ImGuiDockNodeFlags dockFlags = ImGuiDockNodeFlags_PassthruCentralNode;
        ImGuiID dockspaceID = ImGui::DockSpaceOverViewport(ImGui::GetID("MyDockSpace"), ImGui::GetMainViewport());
        if (ImGui::DockBuilderGetNode(dockspaceID) == nullptr || ImGui::DockBuilderGetNode(dockspaceID)->ChildNodes[0] == 0) {
            DockBuilder::setDefaultLayout(dockspaceID);
        }
        MainMenuBar::drawMainMenuBar(editorContext);
        for (auto& panel : m_Panels)
        {
            if (panel->m_IsOpen)
            {
                panel->onImGuiRender(editorContext);
            }
        }
        drawChart();
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

    bool GuiRenderer::isPanelOpen(std::string_view name) const {
        for (const auto& panel : m_Panels) {
            if (panel->getName() && std::string_view(panel->getName()) == name) {
                return panel->m_IsOpen;
            }
        }
        return false;
    }
    void GuiRenderer::setPanelOpen(std::string_view name, bool open) {
        for (auto& panel : m_Panels) {
            if (panel->getName() && std::string_view(panel->getName()) == name) {
                panel->m_IsOpen = open;
                return;
            }
        }
    }
    std::vector<std::string_view> GuiRenderer::getPanelNames() const{
        std::vector<std::string_view> names;
        names.reserve(m_Panels.size());
        for (const auto& panel : m_Panels) {
            if (panel->getName()) {
                names.emplace_back(panel->getName());
            }
        }
        return names;
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
}