#include "pch.h"
#include "GuiManager.h"
#include "SubsystemManager.h"
#include "Renderer.h"

void GuiManager::init(SDL_Window* window) {
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

void GuiManager::update() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();
    drawNavBar();
    drawNotification();
    drawInformation();
    drawChart();
    drawMeasurementDisplay();
}

void GuiManager::drawViewportGUI(unsigned int framebufferTexture, ImVec2 framebufferSize)
{
    ImGuiWindowFlags windowFlags = 0;
    windowFlags |= ImGuiWindowFlags_NoResize;
    windowFlags |= ImGuiWindowFlags_NoMove;
    windowFlags |= ImGuiWindowFlags_NoCollapse;
    windowFlags |= ImGuiWindowFlags_NoScrollbar;
    windowFlags |= ImGuiWindowFlags_NoScrollWithMouse;
    ImGui::SetNextWindowPos(getViewportWindowPos());
    ImGui::SetNextWindowSize(getViewportWindowSize());
    ImGuiStyle& style = ImGui::GetStyle();
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("Viewport", 0, windowFlags);
    ImGui::Image((void*)(intptr_t)framebufferTexture, ImVec2(framebufferSize.x, framebufferSize.y), ImVec2(0, 0), ImVec2(1, 1));
    ImGui::End();
    ImGui::PopStyleVar();
}

float GuiManager::getMenuBarHeight()
{
    return m_MenuBarHeight;
}

void GuiManager::loadFont()
{
    ImGuiIO& io = ImGui::GetIO();(void)io;

    io.Fonts->AddFontFromFileTTF("../assets/fonts/Roboto_Condensed-Black.ttf");
    ImFont* font = io.Fonts->AddFontFromFileTTF("../assets/fonts/Roboto_Condensed-Black.ttf");
    if (font == nullptr) {
        throw std::runtime_error("failed loading Roboto font");
    }
}

void GuiManager::drawNavBar()
{
    ImGuiStyle& style = ImGui::GetStyle();
    style.FramePadding.y = 9.0f;
    style.WindowBorderSize = 0.0f;
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBackground;
    if (ImGui::BeginMainMenuBar())
    {
        m_MenuBarHeight = ImGui::GetWindowSize().y;
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
            if (ImGui::MenuItem("Licenses")) {}
            if (ImGui::MenuItem("SDK's")) {}
            if (ImGui::MenuItem("Github")) {}
            if (ImGui::MenuItem("Version")) {}
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}

void GuiManager::drawNotification()
{
    ImVec2 windowSize = ImVec2(m_WidgetWidth, m_WindowHeight - m_MenuBarHeight - m_MarginDefault * 2);
    ImVec2 newPos = getNewWindowPos(Margin(0.0f, m_MarginDefault, m_MarginDefault, m_MarginDefault), windowSize, Alignment::TopRight);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, m_GuiColor);
    ImGui::SetNextWindowPos(newPos);
    ImGui::SetNextWindowSize(windowSize);
    ImGui::Begin("Notifications", 0, m_WindowFlags);
    ImGui::End();
    ImGui::PopStyleColor();
}

void GuiManager::drawInformation()
{
    ImVec2 windowSize = ImVec2(m_WidgetWidth, m_WindowHeight * 0.55f);
    ImVec2 newPos = getNewWindowPos(Margin(m_MarginDefault, 0, m_MarginDefault, m_MarginDefault), windowSize, Alignment::TopLeft);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, m_GuiColor);
    ImGui::SetNextWindowPos(newPos);
    ImGui::SetNextWindowSize(windowSize);
    ImGui::Begin("Information", 0, m_WindowFlags);
    ImGui::End();
    ImGui::PopStyleColor();
}

void GuiManager::drawChart() {
    ImVec2 windowSize = ImVec2(m_WidgetWidth, m_WindowHeight * 0.35f);
    ImVec2 newPos = getNewWindowPos(Margin(m_MarginDefault, 0, m_MarginDefault, 0), windowSize, Alignment::BottomLeft);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, m_GuiColor);
    ImGui::SetNextWindowPos(newPos);
    ImGui::SetNextWindowSize(windowSize);
    ImGui::Begin("Chart", 0, m_WindowFlags);
    ImGui::End();
    ImGui::PopStyleColor();
}

void GuiManager::drawMeasurementDisplay() {
    ImVec2 windowSize = ImVec2(m_WindowWidth - m_MarginDefault * 2 - 2 * m_WidgetWidth - 100, m_WindowHeight * 0.35f);
    ImVec2 newPos = getNewWindowPos(Margin(m_MarginDefault, 0, m_MarginDefault, 0), windowSize, Alignment::CenterBottom);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, m_GuiColor);
    ImGui::SetNextWindowPos(newPos);
    ImGui::SetNextWindowSize(windowSize);
    ImGui::Begin("Measurement", 0, m_WindowFlags);
    ImGui::Text("PM2.5");
    ImGui::SameLine();
    ImGui::Text("PM5");
    ImGui::SameLine();
    ImGui::Text("PM10");
    ImGui::End();
    ImGui::PopStyleColor();
}

ImVec2 GuiManager::getNewWindowPos(Margin margin, ImVec2 windowSize, Alignment alignment)
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

ImVec2 GuiManager::getViewportWindowPos()
{
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
ImVec2 GuiManager::getViewportWindowSize() {
    return m_ViewportSize;
}

void GuiManager::setViewportSize() {
    if (m_WindowHeight == 0 || m_WindowWidth == 0 || m_WidgetWidth == 0) {
        throw std::runtime_error("Window width/height or widgetWidth can't be zero");
    }
    ImGuiStyle& style = ImGui::GetStyle();
    float viewPortX = std::floor(m_WindowWidth - m_MarginDefault * 2 - 2 * m_WidgetWidth - 100);
    float viewPortY = std::floor(m_WindowHeight*0.55f);
    m_ViewportSize = ImVec2(viewPortX, viewPortY);
}

void GuiManager::setFlags()
{
    ImGuiIO& io = ImGui::GetIO();(void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    m_WindowFlags |= ImGuiWindowFlags_NoCollapse;
    m_WindowFlags |= ImGuiWindowFlags_NoResize;
}

void GuiManager::setStyle() {
    ImGuiStyle& style = ImGui::GetStyle();
    float mainScale = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());
    style.ScaleAllSizes(mainScale);
    style.FontScaleDpi = mainScale;
    m_GuiColor = ImVec4(0.278, 0.278, 0.529,1.0);
}