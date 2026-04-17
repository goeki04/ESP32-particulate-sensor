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
        std::string                   inputBuf;
        std::vector<std::string>      items;
        std::vector<std::string>      history;
        int                           historyPos;
        ImGuiTextFilter               filter;
        bool                          autoScroll;
        bool                          scrollToBottom;

        std::map<std::string_view, CommandProfile> commandRegistry;

        void initRegistry() {
            commandRegistry["network"] = {
                { Flags::info }, 
                [this](const CommandLine& cl) { printNetworkStatus(cl); }
            };

            commandRegistry["help"] = {
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
            items.push_back(std::vformat(fmt, std::make_format_args(args...)));
            scrollToBottom = true;
        }
        void execCommand(CommandLine line);

        static int textEditCallbackStub(ImGuiInputTextCallbackData* data);
        int textEditCallback(ImGuiInputTextCallbackData* data);
    };
}