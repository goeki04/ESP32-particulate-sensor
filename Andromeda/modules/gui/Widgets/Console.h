#pragma once

/**
 * @file Console.h
 * @brief A small command interpreter and log buffer powering the editor's debug console.
 */

#include <imgui.h>
#include <vector>
#include <set>
#include <map>
#include <functional>
#include <string_view>
#include <format>
#include <string>
namespace Andromeda::Gui::Console {

    /**
     * @namespace Andromeda::Gui::Console::Flags
     * @brief String constants for recognized command-line flags.
     */
    namespace Flags {
        constexpr std::string_view info = "-info"; ///< The "-info" flag, requesting verbose/info output.
    }

    /**
     * @struct CommandLine
     * @brief A parsed console input: the command name plus any flags that followed it.
     */
    struct CommandLine {
        std::string_view command;        ///< The command keyword (e.g. "network", "help").
        std::vector<std::string> flags;  ///< The flags supplied after the command (e.g. "-info").
    };

    /**
     * @struct CommandProfile
     * @brief Describes a registered command: which flags it accepts and what it does.
     */
    struct CommandProfile {
        std::set<std::string_view> allowedFlags;          ///< The set of flags this command recognizes.
        std::function<void(const CommandLine&)> action;   ///< Callback invoked to execute the command.
    };

    /**
     * @struct AppConsole
     * @brief Holds the state and behavior of the interactive console widget.
     *
     * @details Combines an input buffer, a scrollable log of output lines, an input history
     *          for arrow-key recall, an ImGui text filter, and a registry mapping command
     *          names to their @c CommandProfile handlers.
     */
    struct AppConsole
    {
        std::string                   inputBuf;        ///< Current text in the input field.
        std::vector<std::string>      items;           ///< Log output lines, displayed top-to-bottom.
        std::vector<std::string>      history;          ///< Previously entered commands, for history recall.
        int                           historyPos;       ///< Current index into @c history (-1 when not browsing).
        ImGuiTextFilter               filter;           ///< Live text filter applied to the log output.
        bool                          autoScroll;       ///< If true, the view follows new log output.
        bool                          scrollToBottom;   ///< One-shot request to scroll to the latest line.

        std::map<std::string_view, CommandProfile> commandRegistry; ///< Registered commands keyed by name.

        /** @brief Populates @c commandRegistry with the built-in commands (e.g. "network", "help"). */
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

        /**
         * @brief Parses a raw input string into a command name and its flags.
         * @param input The raw text entered by the user.
         * @return The parsed @c CommandLine.
         */
        static CommandLine parseInput(std::string_view input);

        /** @brief Built-in "help" command: prints usage information to the log. */
        void printHelp() {
            addLog("Help me");
        }

        /**
         * @brief Built-in "network" command: prints the current network status to the log.
         * @param cl The parsed command line (including any flags).
         */
        void printNetworkStatus(const CommandLine& cl) {
            addLog("NETWORK STATUS: ");
        }

        /** @brief Constructs the console and initializes its command registry/state. */
        AppConsole();
        /** @brief Destroys the console, releasing any owned resources. */
        ~AppConsole();

        /** @brief Clears all output lines from the log. */
        void clearLog();

        /**
         * @brief Appends a formatted line to the log and requests a scroll to the bottom.
         * @tparam Args Format argument types.
         * @param fmt A std::format-style format string.
         * @param args Arguments substituted into @p fmt.
         */
        template<typename... Args>
        void addLog(const std::string_view fmt, Args&&... args)
        {
            items.push_back(std::vformat(fmt, std::make_format_args(args...)));
            scrollToBottom = true;
        }

        /**
         * @brief Looks up and executes a parsed command via its registered handler.
         * @param line The parsed command line to execute.
         */
        void execCommand(CommandLine line);

        /**
         * @brief Static trampoline matching ImGui's input-text callback signature.
         * @details Forwards to the instance method @c textEditCallback() via @c data->UserData.
         * @param data The ImGui callback data.
         * @return Status code as required by ImGui (0 to continue).
         */
        static int textEditCallbackStub(ImGuiInputTextCallbackData* data);

        /**
         * @brief Handles input-field callbacks such as history navigation and tab completion.
         * @param data The ImGui callback data.
         * @return Status code as required by ImGui (0 to continue).
         */
        int textEditCallback(ImGuiInputTextCallbackData* data);
    };
}