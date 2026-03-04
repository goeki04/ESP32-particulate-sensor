#include "pch.h"
#include "GuiRenderer.h"
#include "SubsystemManager.h"
#include "Renderer.h"
#include "camera.h"
#include "ResourceManager.h"
#include "panels.h"
#include "Console.h"
#include "util.h"
ImVec2 Gui::GuiRenderer::s_ViewportSize = ImVec2(0.0f,0.0f);
bool Gui::GuiRenderer::s_ViewportFocused = false;
bool Gui::GuiRenderer::m_ShowVersion = false;
bool Gui::GuiRenderer::m_HasLastHitpoint = false;
glm::vec3 Gui::GuiRenderer::m_LastHitPoint{ 0.0f };
void Gui::GuiRenderer::init(SDL_Window* window, ResourceManager* rm,ECS::ComponentRegistry* registry) {
    m_ResourceManager = rm;
    m_Registry = registry;
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    setFlags();
    setStyle();
    loadFont();
    SDL_GetWindowSizeInPixels(window, &m_WindowWidth, &m_WindowHeight);
    m_WidgetWidth = m_WindowWidth * 0.125;
    m_MarginDefault = m_WindowHeight * 0.0225;
    setViewportSize();

}
void Gui::GuiRenderer::update() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();
    ImGuiDockNodeFlags dockFlags = ImGuiDockNodeFlags_PassthruCentralNode;
    ImGui::DockSpaceOverViewport(0,ImGui::GetMainViewport(),dockFlags);
    Panels::drawNavBar(*this);
    drawNotification();
    Panels::drawDeviceHierarchy(*this);
    drawChart();
    drawBottomWindow();
}

float Gui::GuiRenderer::getMenuBarHeight()
{
    return m_MenuBarHeight;
}
void Gui::GuiRenderer::loadFont()
{
    ImGuiIO& io = ImGui::GetIO();(void)io;
    ImFont* font = io.Fonts->AddFontFromFileTTF(ASSET_PATH "fonts/Roboto-Regular.ttf",18.0f);
    if (font == nullptr) {
        throw std::runtime_error("failed loading Roboto font");
    }
    io.FontDefault = font;
}
void Gui::GuiRenderer::OpenFolder() const
{
#if defined(_WIN32)
    auto targetDirectory =  std::filesystem::current_path().parent_path() / "Licenses";
    std::string cmd = "explorer \"" + targetDirectory.string() + "\"";
    system(cmd.c_str());

#endif
}
void Gui::GuiRenderer::drawNotification()
{
    ImVec2 windowSize = ImVec2(m_WidgetWidth, m_WindowHeight - m_MenuBarHeight - m_MarginDefault * 2);
    ImVec2 newPos = getNewWindowPos(Margin(0.0f, m_MarginDefault, m_MarginDefault, m_MarginDefault), windowSize, Alignment::TopRight);
    ImGui::SetNextWindowPos(newPos, ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(windowSize, ImGuiCond_FirstUseEver);
    ImGui::Begin("Notifications", 0, m_WindowFlags);
    ImGui::End();
}

void Gui::GuiRenderer::drawChart() {
    ImVec2 windowSize = ImVec2(m_WidgetWidth, m_WindowHeight * 0.35f);
    ImVec2 newPos = getNewWindowPos(Margin(m_MarginDefault, 0, m_MarginDefault, 0), windowSize, Alignment::BottomLeft);
    ImGui::SetNextWindowPos(newPos, ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(windowSize, ImGuiCond_FirstUseEver);
    ImGui::Begin("Chart", 0, m_WindowFlags);
    ImGui::End();
}
void Gui::GuiRenderer::drawBottomWindow() {
    ImVec2 windowSize = ImVec2(m_WindowWidth - m_MarginDefault * 2 - 2 * m_WidgetWidth - 100, m_WindowHeight * 0.35f);
    ImVec2 newPos = getNewWindowPos(Margin(m_MarginDefault, 0, m_MarginDefault, 0), windowSize, Alignment::CenterBottom);
    float windowPadding = windowSize.x * 0.1f;
    ImGui::SetNextWindowPos(newPos, ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(windowSize, ImGuiCond_FirstUseEver);
    ImGui::Begin("Device Browser", 0, m_WindowFlags | ImGuiWindowFlags_NoTitleBar  | ImGuiWindowFlags_MenuBar);
    ImVec2 widgetSize = ImVec2(140, 0);
    static int activeTab = 0;
    if (ImGui::BeginMenuBar())
    {
        ImGui::SetCursorPosX(0.0f);
        if (ImGui::BeginTabBar("DeviceBrowser"))
        {
            if (ImGui::BeginTabItem("Device Browser"))
            {
                activeTab = 0;
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }
        ImGui::EndMenuBar();
    }

    ImGui::End();
}
void Gui::GuiRenderer::OpenURL(const std::string& url)
{
   SDL_OpenURL(url.c_str());
}
ImVec2 Gui::GuiRenderer::getNewWindowPos(Margin margin, ImVec2 windowSize, Alignment alignment)
{
    float containerHeight = m_WindowHeight - m_MenuBarHeight;
    float containerWidth = m_WindowWidth;
    ImVec2 newPos = ImVec2(0, 0);

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
ImVec2 Gui::GuiRenderer::getViewportWindowPos()
{
    using namespace util;
    ImVec2 viewportSize = getViewportWindowSize();
    ImVec2 viewportPos = getNewWindowPos(Margin(0.0f, 0.0f, 0.0f, m_MarginDefault), viewportSize, Alignment::CenterTop);
    if (viewportSize == ImVec2(0, 0)) {
        throw std::runtime_error("Viewport has a size of 0!");
    }
    return viewportPos;
}
/// <summary>
/// Used to set window and framebuffer size
/// </summary>
/// <returns>Viewport size in pixels</returns>
ImVec2 Gui::GuiRenderer::getViewportWindowSize() {
    return s_ViewportSize;
}
void Gui::GuiRenderer::setViewportSize() const {
    if (m_WindowHeight == 0 || m_WindowWidth == 0 || m_WidgetWidth == 0) {
        throw std::runtime_error("Window width/height or widgetWidth can't be zero");
    }
    ImGuiStyle& style = ImGui::GetStyle();
    float viewPortX = std::floor(m_WindowWidth - m_MarginDefault * 2 - 2 * m_WidgetWidth - 100);
    float viewPortY = std::floor(m_WindowHeight*0.55f);
    s_ViewportSize = ImVec2(viewPortX, viewPortY);
}
void Gui::GuiRenderer::destroy() {
    ImGui::DestroyPlatformWindows();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    if (ImGui::GetCurrentContext()) {
        ImGui::DestroyContext();
    }
}
void Gui::GuiRenderer::setFlags()
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
void Gui::GuiRenderer::setStyle() {
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