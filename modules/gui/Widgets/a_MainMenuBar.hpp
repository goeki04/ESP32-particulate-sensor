#pragma once
#include "imgui.h"
namespace Andromeda::Gui{
    struct EditorContext;
    class MainMenuBar{
    public:
        static void drawMainMenuBar(EditorContext& ctx);
    private:
        static void drawFileMenu(EditorContext& ctx);
        static void drawDebugMenu();
        static void drawInfoMenu();
        static void drawWindowMenu(EditorContext& ctx);
    };
}