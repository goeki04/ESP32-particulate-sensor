#include "pch.h"
#include "GuiManager.h"
#include "SubsystemManager.h"
#include "GuiManager.h"
#include "Renderer.h"
#include "camera.h"
ImVec2 GuiManager::s_ViewportSize = ImVec2(0.0f,0.0f);
bool GuiManager::s_ViewportFocused = false;
bool GuiManager::m_ShowVersion = false;
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
    drawDeviceBrowser();
}

void GuiManager::drawViewportGUI(unsigned int framebufferTexture, ImVec2 framebufferSize)
{
    SDL_Window* mainWindow = SDL_GetMouseFocus();
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
    s_ViewportFocused = ImGui::IsWindowFocused() ? true : false;
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

    io.Fonts->AddFontFromFileTTF("../assets/fonts/Roboto_Condensed-Black.ttf",24.0f);
    ImFont* font = io.Fonts->AddFontFromFileTTF("../assets/fonts/Roboto_Condensed-Black.ttf",24.0f);
    if (font == nullptr) {
        throw std::runtime_error("failed loading Roboto font");
    }
}

void GuiManager::drawNavBar()
{
    ImGuiStyle& style = ImGui::GetStyle();
    style.FramePadding.y = 9.0f;
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
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
            if (ImGui::MenuItem("Licenses")) {
                OpenFolder();
            }
            if (ImGui::MenuItem("SDK")) {
                OpenURL("https://www.bosch-sensortec.com/software-tools/software/previous-sdk-bmv-080-versions/");
            }
            if (ImGui::MenuItem("Github")) {
                OpenURL("https://github.com/goeki04/ESP32-particulate-sensor");
            }
            if (ImGui::MenuItem("Version")) {
                m_ShowVersion = true;
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
        ImGui::PopStyleVar();
        if (m_ShowVersion) {
            ImGui::SetNextWindowSize(ImVec2(m_WidgetWidth, m_WidgetWidth));
            ImGui::Begin("Version", &m_ShowVersion,m_WindowFlags);
            ImGui::TextUnformatted("App version: V1.0.0");
            std::string osName = "OS: " + std::string(g_os);
            ImGui::TextUnformatted(osName.c_str());
            std::string arch = "Arch: " + std::string(g_arch);
            ImGui::TextUnformatted(arch.c_str());
            ImGui::TextUnformatted("Dev: Goekdeniz Koeksal");
            ImGui::TextUnformatted(m_ImguiVersion.c_str());

            int major = SDL_MAJOR_VERSION;
            int minor = SDL_MINOR_VERSION;
            int patch = SDL_MICRO_VERSION;
            std::string sdlVersion = std::format("SDL {}.{}.{}", major, minor, patch);
            ImGui::TextUnformatted(sdlVersion.c_str());
            ImGui::End();
        }
    }
}
void GuiManager::OpenFolder()
{
#if defined(_WIN32)
    auto targetDirectory =  std::filesystem::current_path().parent_path() / "Licenses";
    std::string cmd = "explorer \"" + targetDirectory.string() + "\"";
    system(cmd.c_str());

#elif defined(__APPLE__)
    std::string cmd = "open \"" + currentPath + "\"";
    system(cmd.c_str());

#else // Linux
    std::string cmd = "xdg-open \"" + currentPath + "\"";
    system(cmd.c_str());

#endif
}

void GuiManager::drawNotification()
{
    ImVec2 windowSize = ImVec2(m_WidgetWidth, m_WindowHeight - m_MenuBarHeight - m_MarginDefault * 2);
    ImVec2 newPos = getNewWindowPos(Margin(0.0f, m_MarginDefault, m_MarginDefault, m_MarginDefault), windowSize, Alignment::TopRight);
    ImGui::SetNextWindowPos(newPos);
    ImGui::SetNextWindowSize(windowSize);
    ImGui::Begin("Notifications", 0, m_WindowFlags);
    ImGui::End();
}

void GuiManager::drawInformation()
{
    ImVec2 windowSize = ImVec2(m_WidgetWidth, m_WindowHeight * 0.55f);
    ImVec2 newPos = getNewWindowPos(Margin(m_MarginDefault, 0, m_MarginDefault, m_MarginDefault), windowSize, Alignment::TopLeft);
    ImGui::SetNextWindowPos(newPos);
    ImGui::SetNextWindowSize(windowSize);
    ImGui::Begin("Devices", 0, m_WindowFlags);
    ImGui::End();
}

void GuiManager::drawChart() {
    ImVec2 windowSize = ImVec2(m_WidgetWidth, m_WindowHeight * 0.35f);
    ImVec2 newPos = getNewWindowPos(Margin(m_MarginDefault, 0, m_MarginDefault, 0), windowSize, Alignment::BottomLeft);
    ImGui::SetNextWindowPos(newPos);
    ImGui::SetNextWindowSize(windowSize);
    ImGui::Begin("Chart", 0, m_WindowFlags);
    ImGui::End();
}

void GuiManager::drawDeviceBrowser() {
    auto CenterText = [](const char* text)
    {
       float columnWidth = ImGui::GetColumnWidth();
       float textWidth = ImGui::CalcTextSize(text).x;
       float offset = (columnWidth - textWidth) * 0.5f;
       ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offset);
       ImGui::TextUnformatted(text);
    };
    ImVec2 windowSize = ImVec2(m_WindowWidth - m_MarginDefault * 2 - 2 * m_WidgetWidth - 100, m_WindowHeight * 0.35f);
    ImVec2 newPos = getNewWindowPos(Margin(m_MarginDefault, 0, m_MarginDefault, 0), windowSize, Alignment::CenterBottom);
    float windowPadding = windowSize.x * 0.1f;
    ImGui::SetNextWindowPos(newPos);
    ImGui::SetNextWindowSize(windowSize);
    ImGui::Begin("Device Browser", 0, m_WindowFlags | ImGuiWindowFlags_NoTitleBar  | ImGuiWindowFlags_MenuBar);
    ImVec2 widgetSize = ImVec2(140, 0);
    static int activeTab = 0; // 0 = Device Browser, 1 = Details Panel

    if (ImGui::BeginMenuBar())
    {
        ImGui::SetCursorPosX(0.0f);
        if (ImGui::BeginTabBar("TopTabs"))
        {
            if (ImGui::BeginTabItem("Device Browser"))
            {
                // Inhalt Device Browser
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Details Panel"))
            {
                // Inhalt Details Panel
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }
        ImGui::EndMenuBar();
    }
    ImGui::End();
}



void GuiManager::OpenURL(const std::string& url)
{
#if defined(_WIN32)
    std::string command = "start " + url;
    system(command.c_str());

#elif defined(__APPLE__)
    std::string command = "open " + url;
    system(command.c_str());

#else   // Linux
    std::string command = "xdg-open " + url;
    system(command.c_str());

#endif
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
    return s_ViewportSize;
}

void GuiManager::setViewportSize() {
    if (m_WindowHeight == 0 || m_WindowWidth == 0 || m_WidgetWidth == 0) {
        throw std::runtime_error("Window width/height or widgetWidth can't be zero");
    }
    ImGuiStyle& style = ImGui::GetStyle();
    float viewPortX = std::floor(m_WindowWidth - m_MarginDefault * 2 - 2 * m_WidgetWidth - 100);
    float viewPortY = std::floor(m_WindowHeight*0.55f);
    s_ViewportSize = ImVec2(viewPortX, viewPortY);
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
    style.WindowRounding = 10.0f;    // Radius
    style.ChildRounding = 10.0f;
    style.FrameRounding = 10.0f;
    style.WindowBorderSize = 1.0f;   // 0 = kein Rand
    ImVec4* colors = style.Colors;
    colors[ImGuiCol_WindowBg]         = ImVec4(0.15f, 0.17f, 0.23f, 1.00f);
    colors[ImGuiCol_ChildBg]          = ImVec4(0.12f, 0.14f, 0.19f, 1.00f);
    colors[ImGuiCol_PopupBg]          = ImVec4(0.10f, 0.12f, 0.18f, 1.00f);
    colors[ImGuiCol_Border]           = ImVec4(0.18f, 0.20f, 0.30f, 0.9f);
    colors[ImGuiCol_FrameBg]          = ImVec4(0.17f, 0.19f, 0.26f, 1.00f);
    colors[ImGuiCol_MenuBarBg]        = ImVec4(0.12f, 0.14f, 0.20f, 1.00f);
    colors[ImGuiCol_TitleBg]          = ImVec4(0.12f, 0.14f, 0.20f, 1.00f);
    colors[ImGuiCol_TitleBgActive]    = ImVec4(0.16f, 0.18f, 0.26f, 1.00f);
    colors[ImGuiCol_Tab]              = ImVec4(0.12f, 0.14f, 0.20f, 1.00f);
    colors[ImGuiCol_TabActive]        = ImVec4(0.16f, 0.18f, 0.26f, 1.00f);
    colors[ImGuiCol_Header]           = ImVec4(0.20f, 0.22f, 0.30f, 1.00f);
    colors[ImGuiCol_HeaderHovered]    = ImVec4(0.26f, 0.28f, 0.38f, 1.00f);
    colors[ImGuiCol_HeaderActive]     = ImVec4(0.30f, 0.32f, 0.44f, 1.00f);

    colors[ImGuiCol_Button]           = ImVec4(0.21f, 0.23f, 0.32f, 1.00f);
    colors[ImGuiCol_ButtonHovered]    = ImVec4(0.28f, 0.30f, 0.40f, 1.00f);
    colors[ImGuiCol_ButtonActive]     = ImVec4(0.32f, 0.35f, 0.46f, 1.00f);

    colors[ImGuiCol_CheckMark]        = ImVec4(0.53f, 0.58f, 0.95f, 1.00f);
    colors[ImGuiCol_SliderGrab]       = ImVec4(0.50f, 0.55f, 0.90f, 1.00f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.57f, 0.62f, 0.98f, 1.00f);

    colors[ImGuiCol_Text]             = ImVec4(0.86f, 0.89f, 0.93f, 1.00f);
    colors[ImGuiCol_TextDisabled]     = ImVec4(0.47f, 0.50f, 0.56f, 1.00f);
}