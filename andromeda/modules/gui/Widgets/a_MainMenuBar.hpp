#pragma once

/**
 * @file a_MainMenuBar.hpp
 * @brief The editor's top application menu bar (File, Debug, Info, Window).
 */

#include "imgui.h"
namespace Andromeda::Gui{
    struct EditorContext;

    /**
     * @class MainMenuBar
     * @brief Renders the main menu bar and its dropdown menus at the top of the editor window.
     * @details A stateless utility class; all rendering is done through static methods that read
     *          from and act upon the shared @c EditorContext.
     */
    class MainMenuBar{
    public:
        /**
         * @brief Renders the complete main menu bar and all of its dropdown menus.
         * @param ctx The shared editor context (used for actions like save/load and panel toggles).
         */
        static void drawMainMenuBar(EditorContext& ctx);
    private:
        /** @brief Renders the "File" menu (e.g. open project, save/load scene, exit). */
        static void drawFileMenu(EditorContext& ctx);
        /** @brief Renders the "Debug" menu (e.g. diagnostic toggles). */
        static void drawDebugMenu();
        /** @brief Renders the "Info" menu (e.g. version, links, about). */
        static void drawInfoMenu();
        /** @brief Renders the "Window" menu, toggling panel visibility via the panel controller. */
        static void drawWindowMenu(EditorContext& ctx);
    };
}