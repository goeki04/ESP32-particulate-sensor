#include "pch.h"
#include "GuiManager.h"
#include "SubsystemManager.h"
#include "Renderer.h"
#include "camera.h"
#include "ResourceManager.h"
#include "util.h"
using namespace util;
ImVec2 GuiManager::s_ViewportSize = ImVec2(0.0f,0.0f);
bool GuiManager::s_ViewportFocused = false;
bool GuiManager::m_ShowVersion = false;
bool GuiManager::m_HasLastHitpoint = false;
glm::vec3 GuiManager::m_LastHitPoint{ 0.0f };
void GuiManager::init(SDL_Window* window, ResourceManager* rm) {
    m_ResourceManager = rm;
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
    drawDeviceHierarchy();
    drawChart();
    drawBottomWindow();
}
void GuiManager::drawViewportGUI(unsigned int framebufferTexture,ImVec2 framebufferSize,float* ImGuiMouseX,float* ImGuiMouseY)
{
    Camera& cam = m_ResourceManager->m_Cam;
    ImGuiWindowFlags windowFlags = 0;
    windowFlags |= ImGuiWindowFlags_NoResize;
    windowFlags |= ImGuiWindowFlags_NoMove;
    windowFlags |= ImGuiWindowFlags_NoCollapse;
    windowFlags |= ImGuiWindowFlags_NoScrollbar;
    windowFlags |= ImGuiWindowFlags_NoScrollWithMouse;
    m_ViewportPos = getViewportWindowPos();
    ImVec2 viewportSize = getViewportWindowSize();
    ImGui::SetNextWindowPos(m_ViewportPos);
    ImGui::SetNextWindowSize(viewportSize);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("Viewport", 0, windowFlags);
    if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
    {
        ImGui::SetWindowFocus();
    }

    if (ImGuiMouseX) *ImGuiMouseX = -1.0f;
    if (ImGuiMouseY) *ImGuiMouseY = -1.0f;

    ImGui::Image((void*)(intptr_t)framebufferTexture, framebufferSize, ImVec2(0, 1), ImVec2(1, 0));

    ImVec2 rectMin = ImGui::GetItemRectMin();
    ImVec2 rectMax = ImGui::GetItemRectMax();
    ImVec2 rectSize = ImVec2(rectMax.x - rectMin.x, rectMax.y - rectMin.y);
    cam.m_ViewportSize = glm::vec2(viewportSize.x,viewportSize.y);
    cam.m_ViewportPos = glm::vec2(m_ViewportPos.x, m_ViewportPos.y);
    ImVec2 mousePos = ImGui::GetMousePos();
    ImVec2 rel = ImVec2(mousePos.x - rectMin.x, mousePos.y - rectMin.y);

    bool hovered = ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem);
    GuiManager::s_ViewportFocused = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows);
    cam.m_HasValidPickRay = hovered;

    cam.m_HasValidPickRay = hovered;

    if (hovered) {
        cam.m_ImGuiMouseX = rel.x;
        cam.m_ImGuiMouseY = rel.y;
        cam.m_framebufferSize = glm::vec2(rectSize.x, rectSize.y);
        cam.m_CursorToWorldRay = cam.cursorToWorldRay();

        if (ImGuiMouseX) *ImGuiMouseX = rel.x;
        if (ImGuiMouseY) *ImGuiMouseY = rel.y;
    }
    else {
        cam.m_ImGuiMouseX = cam.m_ImGuiMouseY = -1.0f;
    }

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

    m_HeaderFont = io.Fonts->AddFontFromFileTTF("../assets/fonts/Roboto_Condensed-Black.ttf",24.0f);
    m_DeviceBrowserFont = io.Fonts->AddFontFromFileTTF("../assets/fonts/Roboto-ExtraLight.ttf",24.0f);
    if (m_HeaderFont == nullptr || m_DeviceBrowserFont == nullptr) {
        throw std::runtime_error("failed loading Roboto font");
    }
    io.FontDefault = m_HeaderFont;
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
void GuiManager::drawDeviceHierarchy()
{
    ImVec2 windowSize = ImVec2(m_WidgetWidth, m_WindowHeight * 0.55f);
    ImVec2 newPos = getNewWindowPos(Margin(m_MarginDefault, 0, m_MarginDefault, m_MarginDefault), windowSize, Alignment::TopLeft);
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 0.0f);
    ImGui::SetNextWindowPos(newPos);
    ImGui::SetNextWindowSize(windowSize);
    ImGui::Begin("Hierarchy", 0, m_WindowFlags);
    auto& entitys = m_ResourceManager->getEntitys();
    ImGui::BeginChild("HierarchyList",ImVec2(0.0f,0.0f),true, ImGuiWindowFlags_NoScrollbar);
    ImGuiListClipper clipper;
    clipper.Begin((int)entitys.size());
    while (clipper.Step())
    {
        for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
        {
            const Entity& e = entitys[i];
            bool isSelected = (m_CurrentSelectedID == e.m_ID);
            if (isSelected) {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.5f, 0.0f, 1.0f));
            }

            if (ImGui::Selectable(e.m_Name.c_str(), isSelected)) {
                m_CurrentSelectedID = e.m_ID;
                bool isClickedRight = ImGui::IsItemClicked(ImGuiMouseButton_Right);
                if (isClickedRight) {
                    std::cout << isClickedRight << std::endl;
                }
                for (auto& entity : m_ResourceManager->getEntitys()) {
                    entity.m_IsSelected = (entity.m_ID == e.m_ID);
                }
            }
            if (ImGui::BeginPopupContextItem())
            {
                m_CurrentSelectedID = e.m_ID;

                ImGui::Text("Context");
                ImGui::Separator();

                if (ImGui::MenuItem("Delete")) {
                    m_ResourceManager->deleteEntityByObject(entitys[i]);
                }

                ImGui::EndPopup();
            }

            if (isSelected) {
                ImGui::PopStyleColor();
            }
        }
    }

    ImGui::EndChild();
    ImGui::PopStyleVar();
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
void GuiManager::drawBottomWindow() {
    ImVec2 windowSize = ImVec2(m_WindowWidth - m_MarginDefault * 2 - 2 * m_WidgetWidth - 100, m_WindowHeight * 0.35f);
    ImVec2 newPos = getNewWindowPos(Margin(m_MarginDefault, 0, m_MarginDefault, 0), windowSize, Alignment::CenterBottom);
    float windowPadding = windowSize.x * 0.1f;
    ImGui::SetNextWindowPos(newPos);
    ImGui::SetNextWindowSize(windowSize);
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
            if (ImGui::BeginTabItem("Details Panel"))
            {
                activeTab = 1;
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }
        ImGui::EndMenuBar();
    }

    if (activeTab == 0) {
        ImGuiWindowFlags flags = m_WindowFlags;

        if (!ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows))
        {
            flags |= ImGuiWindowFlags_NoScrollWithMouse;
        }
        ImGui::BeginChild("GridArea", ImVec2(0, 0), true,flags);
        drawDeviceBrowser();
        ImGui::EndChild();
    }
    else if (activeTab == 1) {
        drawDetailsPanel();
    }
    ImGui::End();
}
void GuiManager::drawDeviceBrowser()
{
    Camera& cam = m_ResourceManager->m_Cam;
    static char query[128] = "";
    ImGui::InputTextWithHint("##search", "Search components...", query, IM_ARRAYSIZE(query));
    ImGui::Spacing();

    ImDrawList* dl = ImGui::GetWindowDrawList();
    ImGuiStyle& style = ImGui::GetStyle();

    const float h = ImGui::GetFrameHeight();
    const ImVec2 tileSize(h * 2.0f, h * 2.0f);

    const float availX = ImGui::GetContentRegionAvail().x;
    const float spacingX = style.ItemSpacing.x;
    const float spacingY = style.ItemSpacing.y;

    int perRow = (int)floor((availX + spacingX) / (tileSize.x + spacingX));
    if (perRow < 1) perRow = 1;

    const int itemCount = m_ResourceManager->getDeviceRecordsSize();
    ImGui::PushFont(m_DeviceBrowserFont);

    bool any_tile_clicked = false;

    for (int idx = 0; idx < itemCount; ++idx)
    {
        const auto& deviceRecord = m_ResourceManager->getDeviceRecords().at(idx);
        GLuint texID = m_ResourceManager->m_DeviceIcons.at(deviceRecord.type).id;

        ImGui::PushID(idx);

        const char* label = deviceRecord.name.c_str();
        ImVec2 labelSize = ImGui::CalcTextSize(label);
        ImVec2 totalSize(tileSize.x, tileSize.y + spacingY + labelSize.y);

        ImVec2 pMin = ImGui::GetCursorScreenPos();
        ImGui::InvisibleButton("tile", totalSize);

        bool hovered = ImGui::IsItemHovered();
        bool active = ImGui::IsItemActive();
        bool clicked = ImGui::IsItemClicked(ImGuiMouseButton_Left);
        bool dragged = ImGui::IsMouseDragging(ImGuiMouseButton_Left, 2.0f);
        bool dragEnded = ImGui::IsItemDeactivated() && ImGui::IsMouseReleased(ImGuiMouseButton_Left);
        if (clicked) {
            m_SelectedIdx = idx;
            any_tile_clicked = true;
        }
        bool selected = (m_SelectedIdx == idx);

        ImVec2 tileMin = pMin;
        ImVec2 tileMax = ImVec2(pMin.x + tileSize.x, pMin.y + tileSize.y);

        if (deviceRecord.type == deviceType::DEFAULT) {
            dl->AddRectFilled(tileMin, tileMax, IM_COL32(41, 46, 66, 255), 4.0f);
        }
        else {
            dl->AddImage((ImTextureID)(intptr_t)texID, tileMin, tileMax);
        }

        if (dragged && active) {
            glm::vec3 hitpoint;
            m_HasLastHitpoint = false;
            if (cam.m_HasValidPickRay && cam.RayIntersectsXZPlane(cam.m_CursorToWorldRay,0.0f,hitpoint)) {
                m_LastHitPoint = hitpoint;
                m_HasLastHitpoint = true;
            }
            ImDrawList* fg = ImGui::GetForegroundDrawList();
            ImVec2 tileSizeDragged = tileSize * 0.75f;
            ImVec2 tMin = ImGui::GetMousePos() - tileSizeDragged * 0.5f;
            ImVec2 tMax = ImVec2(tMin + tileSizeDragged);
            fg->AddImage((ImTextureID)(intptr_t)texID, tMin, tMax);
        }
        if (dragEnded && cam.m_HasValidPickRay && m_HasLastHitpoint) {
            Transform transform;
            transform.position = m_LastHitPoint;
            m_ResourceManager->addEntity(deviceRecord.id,deviceRecord.name,transform);
        }
        if (hovered || active)
            dl->AddRect(tileMin, tileMax, IM_COL32(255, 255, 255, 60), 4.0f, 0, 1.5f);

        if (selected) {
            dl->AddRectFilled(tileMin, tileMax, IM_COL32(40, 120, 255, 80), 4.0f);
            dl->AddRect(tileMin, tileMax, IM_COL32(40, 120, 255, 200), 4.0f, 0, 2.0f);

            const float glow = 3.0f;
            dl->AddRect(tileMin - ImVec2(glow, glow),
                tileMax + ImVec2(glow, glow),
                IM_COL32(40, 120, 255, 90),
                6.0f, 0, 2.0f);
        }

        float textX = tileMin.x + (tileSize.x - labelSize.x) * 0.5f;
        float textY = tileMax.y + spacingY;
        dl->AddText(ImVec2(textX, textY), IM_COL32(255, 255, 255, 255), label);

        int col = idx % perRow;
        if (col != perRow - 1)
            ImGui::SameLine(0.0f, spacingX);

        ImGui::PopID();
    }

    if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) &&
        !any_tile_clicked &&
        !ImGui::IsAnyItemHovered() &&
        ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup))
    {
        m_SelectedIdx = -1;
    }

    ImGui::PopFont();
}
void GuiManager::drawDetailsPanel()
{
}
void GuiManager::OpenURL(const std::string& url)
{
   SDL_OpenURL(url.c_str());
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
void GuiManager::setViewportSize() const {
    if (m_WindowHeight == 0 || m_WindowWidth == 0 || m_WidgetWidth == 0) {
        throw std::runtime_error("Window width/height or widgetWidth can't be zero");
    }
    ImGuiStyle& style = ImGui::GetStyle();
    float viewPortX = std::floor(m_WindowWidth - m_MarginDefault * 2 - 2 * m_WidgetWidth - 100);
    float viewPortY = std::floor(m_WindowHeight*0.55f);
    s_ViewportSize = ImVec2(viewPortX, viewPortY);
}
void GuiManager::destroy() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();
}
void GuiManager::setFlags()
{
    ImGuiIO& io = ImGui::GetIO();(void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    m_WindowFlags |= ImGuiWindowFlags_NoCollapse;
    m_WindowFlags |= ImGuiWindowFlags_NoResize;
}
/// <summary>
/// color scheme
/// </summary>
void GuiManager::setStyle() {
    ImGuiStyle& style = ImGui::GetStyle();
    float mainScale = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());
    style.ScaleAllSizes(mainScale);
    style.WindowRounding = 10.0f;
    style.ChildRounding = 10.0f;
    style.FrameRounding = 10.0f;
    style.WindowBorderSize = 1.0f;
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