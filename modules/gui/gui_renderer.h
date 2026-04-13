#pragma once
#include "a_graphics_base.hpp"
#include <vector>
#include <string>

#include "a_EditorContext.hpp"
#include "a_registry.hpp"
#include "a_guiTypes.hpp"
#include "resource_manager.h"
#include "a_texture.hpp"
#include "a_EditorPanel.hpp"
class IDeviceProvider;
class SceneManager;
namespace Andromeda::amath {
    struct CameraData;
}

inline ImVec2 operator+(const ImVec2& a, const ImVec2& b) {
    return {a.x + b.x, a.y + b.y};
}
inline ImVec2 operator-(const ImVec2& a, const ImVec2& b) {
    return {a.x - b.x, a.y - b.y};
}
inline bool operator==(const ImVec2& a, const ImVec2& b) {
    if (a.x == b.x && a.y == b.y) return true;
    return false;
}
inline ImVec2 operator*(const ImVec2& a, const float b) {
    return {a.x * b, a.y * b};
}
namespace Andromeda::Gui {

    class GuiRenderer {
    public:
        void init(EditorContext& editorContext);
        void update(const ViewportDrawInfo& vpInfo, EditorContext& editorContext);
        static void EndFrame(SDL_Window* window);
        void destroy();
        std::vector<std::unique_ptr<EditorPanel>> m_Panels;

        static void loadFont();

        std::vector<SDL_Surface> m_DeviceIcons;

        bool m_ConsoleOpen = false;
        template<typename T, typename... Args>
        void registerPanel(Args&&... args) {
            m_Panels.push_back(std::make_unique<T>(std::forward<Args>(args)...));
        }
        static void OpenFolder();
        static void OpenURL(const std::string& url);
        void drawChart() const;
        static void setFlags();
        static void setStyle();
    };
}