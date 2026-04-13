#pragma once

#include <imgui.h>
#include <vector>
#include <set>
#include <map>
#include <functional>
#include <string_view>
#include <format>
#include <string>
namespace Andromeda::Gui::Console {
    
    namespace Flags {
        constexpr std::string_view info = "-info";
    }

    struct CommandLine {
        std::string_view command;
        std::vector<std::string> flags;
    };

    struct CommandProfile {
        std::set<std::string_view> allowedFlags;
        std::function<void(const CommandLine&)> action;
    };

    struct AppConsole
    {
        std::string                   m_InputBuf;
        std::vector<std::string>      m_Items;
        std::vector<std::string>      m_History;
        int                           m_HistoryPos;
        ImGuiTextFilter               m_Filter;
        bool                          m_AutoScroll;
        bool                          m_ScrollToBottom;

        std::map<std::string_view, CommandProfile> m_CommandRegistry;

        void initRegistry() {
            m_CommandRegistry["network"] = {
                { Flags::info }, 
                [this](const CommandLine& cl) { printNetworkStatus(cl); }
            };

            m_CommandRegistry["help"] = {
                { Flags::info },
                [this](const CommandLine& cl) { printHelp(); }
            };
        }

        static CommandLine parseInput(std::string_view input);
        void printHelp() {
            addLog("Help me");
        }

        void printNetworkStatus(const CommandLine& cl) {
            addLog("NETWORK STATUS: ");
        }

        AppConsole();
        ~AppConsole();
        void clearLog();
        template<typename... Args>
        void addLog(const std::string_view fmt, Args&&... args)
        {
            m_Items.push_back(std::vformat(fmt, std::make_format_args(args...)));
            m_ScrollToBottom = true;
        }
        void draw(const char* title, bool* pOpen);
        void execCommand(CommandLine line);

        static int textEditCallbackStub(ImGuiInputTextCallbackData* data);
        int textEditCallback(ImGuiInputTextCallbackData* data);
    };
}