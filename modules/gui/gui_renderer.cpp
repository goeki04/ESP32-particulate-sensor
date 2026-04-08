
#include "gui_renderer.h"
#include "panels.h"
#include <stdexcept>
#include <algorithm>
#include "window_manager.hpp"
#include <filesystem>
namespace Andromeda::Gui {
    ImVec2 Gui::GuiRenderer::s_ViewportSize = ImVec2(0.0f, 0.0f);
    bool Gui::GuiRenderer::s_ViewportFocused = false;
    bool Gui::GuiRenderer::m_ShowVersion = false;
    bool Gui::GuiRenderer::m_HasLastHitpoint = false;
    vec3 Gui::GuiRenderer::m_LastHitPoint{ 0.0f };

    void Gui::GuiRenderer::init(const GuiRendererConfig &guiConfig) {
        assert(guiConfig.cam &&"CameraData is nullptr in GuiRenderer::Init()");
        assert(guiConfig.window && "window is nullptr in GuiRenderer:Init()");
        assert(guiConfig.registry && "registry is nullptr in GuiRenderer::Init()");
        assert(guiConfig.sceneManager && "sceneManager is nullptr in GuiRenderer::Init()");
        assert(guiConfig.resource && "resource is nullptr in GuiRenderer::Init()");
        m_Cam = guiConfig.cam;
        m_Registry = guiConfig.registry;
        m_DeviceProvider = guiConfig.dp;
        m_SceneManager = guiConfig.sceneManager;
        m_ResourceManager = guiConfig.resource;
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui_ImplSDL3_InitForOpenGL(guiConfig.window, guiConfig.sdl_gl_context);
        ImGui_ImplOpenGL3_Init(guiConfig.glsl_version);
        ImGui::StyleColorsDark();
        setFlags();
        setStyle();
        loadFont();
        SDL_GetWindowSizeInPixels(guiConfig.window, &m_WindowWidth, &m_WindowHeight);
        m_WidgetWidth = static_cast<float>(m_WindowWidth) * 0.125f;
        m_MarginDefault = static_cast<float>(m_WindowHeight) * 0.0225f;
        setViewportSize();
    }
    void GuiRenderer::update(const ViewportDrawInfo& vpInfo) {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();
        constexpr ImGuiDockNodeFlags dockFlags = ImGuiDockNodeFlags_PassthruCentralNode;
        ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport(), dockFlags);
        Panels::drawNavBar(*this);

        drawToolBar(m_ResourceManager->getEditorIconID("box"));
        Panels::drawDeviceBrowser(*this);
        Panels::drawDeviceHierarchy(*this);
        Panels::drawDetails(*this);
        drawChart();
        Panels::drawViewportGUI(*this,vpInfo);
        EndFrame(Window::g_Window);
    }

    void GuiRenderer::drawToolBar(const u32& textureID)
    {

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

    float GuiRenderer::getMenuBarHeight() const
    {
        return m_MenuBarHeight;
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
        const ImVec2 windowSize = ImVec2(m_WidgetWidth, m_WindowHeight * 0.35f);
        const ImVec2 newPos = getNewWindowPos(Margin(m_MarginDefault, 0, m_MarginDefault, 0), windowSize, Alignment::BottomLeft);
        ImGui::SetNextWindowPos(newPos, ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(windowSize, ImGuiCond_FirstUseEver);
        ImGui::Begin("Chart", nullptr, m_WindowFlags);
        ImGui::End();
    }
    void GuiRenderer::OpenURL(const std::string& url)
    {
        SDL_OpenURL(url.c_str());
    }
    ImVec2 GuiRenderer::getNewWindowPos(Margin margin, ImVec2 windowSize, const Alignment alignment) const
    {
        const float containerHeight = m_WindowHeight - m_MenuBarHeight;
        const float containerWidth = m_WindowWidth;
        auto newPos = ImVec2(0, 0);

        if (windowSize.x <= 0 || windowSize.y <= 0)
        {
            windowSize.x = 100;
            windowSize.y = 100;
            printf("Warning: Invalid window size. Reset to 100x100.\n");
        }

        if (margin.left < 0) margin.left = 0;
        if (margin.top < 0) margin.top = 0;
        if (margin.right < 0) margin.right = 0;
        if (margin.bottom < 0) margin.bottom = 0;
        // xPos     
        switch (alignment)
        {
        case Alignment::TopLeft:
        case Alignment::BottomLeft:
            newPos.x = margin.left;
            break;

        case Alignment::TopRight:
        case Alignment::BottomRight:
            newPos.x = containerWidth - windowSize.x - margin.right;
            break;

        case Alignment::Center:
        case Alignment::CenterTop:
        case Alignment::CenterBottom:
            if (margin.left != 0 && margin.right != 0)
                newPos.x = (containerWidth - windowSize.x + margin.left - margin.right) / 2.0f;
            else
                newPos.x = (containerWidth - windowSize.x) / 2.0f;
            break;
        }
        // yPos
        switch (alignment)
        {
        case Alignment::TopLeft:
        case Alignment::TopRight:
        case Alignment::CenterTop:
            newPos.y = margin.top + m_MenuBarHeight;
            break;
        case Alignment::BottomLeft:
        case Alignment::BottomRight:
        case Alignment::CenterBottom:
            newPos.y = containerHeight - windowSize.y - margin.bottom + m_MenuBarHeight;
            break;
        case Alignment::Center:
            if (margin.top != 0 && margin.bottom != 0)
                newPos.y = m_MenuBarHeight + (containerHeight - windowSize.y + margin.top - margin.bottom) / 2.0f;
            else
                newPos.y = m_MenuBarHeight + (containerHeight - windowSize.y) / 2.0f;
            break;
        }

        newPos.x = std::clamp(newPos.x, 0.0f, containerWidth - windowSize.x);
        newPos.y = std::clamp(newPos.y, m_MenuBarHeight, containerHeight + m_MenuBarHeight - windowSize.y);
        return newPos;
    }

    ImVec2 GuiRenderer::getViewportWindowPos() const
    {
        const vec2 vpSize = getViewportWindowSize();
        const auto viewportSize = ImVec2(vpSize.x,vpSize.y);
        const ImVec2 viewportPos = getNewWindowPos(Margin(0.0f, 0.0f, 0.0f, m_MarginDefault), viewportSize, Alignment::CenterTop);
        if (viewportSize == ImVec2(0, 0)) {
            throw std::runtime_error("Viewport has a size of 0!");
        }
        return viewportPos;
    }
    /// <summary>
    /// Used to set window and framebuffer size
    /// </summary>
    /// <returns>Viewport size in pixels</returns>
    vec2 GuiRenderer::getViewportWindowSize() {
        const auto viewportSize = vec2(s_ViewportSize.x, s_ViewportSize.y);
        return viewportSize;
    }
    void GuiRenderer::setViewportSize() const {
        if (m_WindowHeight == 0 || m_WindowWidth == 0 || m_WidgetWidth == 0) {
            throw std::runtime_error("Window width/height or widgetWidth can't be zero");
        }
        const float viewPortX = std::floor(m_WindowWidth - m_MarginDefault * 2 - 2 * m_WidgetWidth - 100);
        const float viewPortY = std::floor(m_WindowHeight * 0.55f);
        s_ViewportSize = ImVec2(viewPortX, viewPortY);
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