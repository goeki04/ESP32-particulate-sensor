#include "console.h"
#include <filesystem>
namespace Andromeda::Gui::Console {

    AppConsole::AppConsole()
    {
        initRegistry();
        clearLog();
        m_HistoryPos = -1;
        m_InputBuf.resize(256,'\0');
        m_AutoScroll = true;
        m_ScrollToBottom = false;
        addLog("Type 'help' for command list.");
    }

    AppConsole::~AppConsole()
    {
        clearLog();
        m_History.clear();
        m_Items.clear();
    }


    void AppConsole::clearLog()
    {
        m_Items.clear();
    }

    CommandLine AppConsole::parseInput(std::string_view input) {
        CommandLine cl;
        size_t space_pos = input.find_first_of(" \t");

        if (space_pos != std::string_view::npos) {
            cl.command = input.substr(0, space_pos);

            std::istringstream iss{ std::string(input.substr(space_pos)) };
            std::string token;
            while (iss >> token) {
                cl.flags.push_back(token);
            }
        }
        else {
            cl.command = input;
        }

        return cl;
    }
    void AppConsole::draw(const char* title, bool* p_open)
    {
        ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_FirstUseEver);
        if (!ImGui::Begin(title, p_open))
        {
            ImGui::End();
            return;
        }

        if (ImGui::BeginPopupContextItem())
        {
            if (ImGui::MenuItem("Close Console"))
                *p_open = false;
            ImGui::EndPopup();
        }

        ImGui::TextWrapped("Enter 'help' for help.");

        if (ImGui::SmallButton("Clear")) { clearLog(); }
        ImGui::SameLine();
        bool copy_to_clipboard = ImGui::SmallButton("Copy");

        ImGui::Separator();

        if (ImGui::BeginPopup("Options"))
        {
            ImGui::Checkbox("Auto-scroll", &m_AutoScroll);
            ImGui::EndPopup();
        }

        if (ImGui::Button("Options")) ImGui::OpenPopup("Options");
        ImGui::SameLine();
        m_Filter.Draw("Filter", 180);
        ImGui::Separator();

        const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
        if (ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footer_height_to_reserve), ImGuiChildFlags_NavFlattened, ImGuiWindowFlags_HorizontalScrollbar))
        {
            if (ImGui::BeginPopupContextWindow())
            {
                if (ImGui::Selectable("Clear")) clearLog();
                ImGui::EndPopup();
            }

            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1));
            if (copy_to_clipboard) ImGui::LogToClipboard();

            for (const auto& item : m_Items)
            {
                if (!m_Filter.PassFilter(item.c_str()))
                    continue;

                ImVec4 color;
                bool has_color = false;

                if (item.find("[E]") != std::string::npos)
                {
                    color = ImVec4(1.0f, 0.4f, 0.4f, 1.0f);
                    has_color = true;
                }
                else if (item.substr(0, 2) == "# ")
                {
                    color = ImVec4(1.0f, 0.8f, 0.6f, 1.0f);
                    has_color = true;
                }

                if (has_color)
                    ImGui::PushStyleColor(ImGuiCol_Text, color);
                ImGui::TextUnformatted(item.c_str());
                if (has_color)
                    ImGui::PopStyleColor();
            }
            if (copy_to_clipboard) ImGui::LogFinish();

            if (m_ScrollToBottom || (m_AutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()))
                ImGui::SetScrollHereY(1.0f);

            m_ScrollToBottom = false;
            ImGui::PopStyleVar();
        }
        ImGui::EndChild();
        ImGui::Separator();

        bool reclaim_focus = false;
        ImGuiInputTextFlags input_text_flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_EscapeClearsAll | ImGuiInputTextFlags_CallbackCompletion | ImGuiInputTextFlags_CallbackHistory;
        if (ImGui::InputText("Input", m_InputBuf.data(), 256, input_text_flags, &AppConsole::textEditCallbackStub, (void*)this))
        {
            std::string_view inputView = m_InputBuf.data();
            size_t start = inputView.find_first_not_of(" \t\r\n");
            size_t end = inputView.find_last_not_of(" \t\r\n");
            if (start != std::string_view::npos)
            {
                std::string_view trimmedCmd = inputView.substr(start, end - start + 1);

                CommandLine cl = parseInput(trimmedCmd);
                execCommand(cl);
            }
            m_InputBuf[0] = '\0';
            reclaim_focus = true;
        }

        ImGui::SetItemDefaultFocus();
        if (reclaim_focus) ImGui::SetKeyboardFocusHere(-1);

        ImGui::End();
    }

    void AppConsole::execCommand(CommandLine line)
    {
        m_HistoryPos = -1;
        std::erase(m_History, std::string(line.command));
        m_History.push_back(std::string(line.command));
        auto it = m_CommandRegistry.find(line.command);
        if (it != m_CommandRegistry.end())
        {
            const auto& profile = it->second;
            bool flagsValid = true;
            for (const auto& f : line.flags)
            {
                if (profile.allowedFlags.find(f) == profile.allowedFlags.end())
                {
                    addLog("[E] Flag {} is not allowed for {}", f,line.command);
                    flagsValid = false;
                }
            }
            if (flagsValid)
                profile.action(line);
        }
        else
            addLog("[E] Unknown command: {}", line.command);
        m_ScrollToBottom = true;
    }

    int AppConsole::textEditCallbackStub(ImGuiInputTextCallbackData* data)
    {
        AppConsole* console = (AppConsole*)data->UserData;
        return console->textEditCallback(data);
    }

    int AppConsole::textEditCallback(ImGuiInputTextCallbackData* data)
    {
        switch (data->EventFlag)
        {
        case ImGuiInputTextFlags_CallbackCompletion:
        {
            const char* word_end = data->Buf + data->CursorPos;
            const char* word_start = word_end;
            while (word_start > data->Buf)
            {
                const char c = word_start[-1];
                if (c == ' ' || c == '\t' || c == ',' || c == ';')
                    break;
                word_start--;
            }

            std::vector<std::string_view> candidates;
            std::string_view current_word(word_start, static_cast<size_t>(word_end - word_start));

            auto char_iequal = [](char a, char b) {
                return std::tolower(static_cast<unsigned char>(a)) == std::tolower(static_cast<unsigned char>(b));
                };

            for (auto const& [name, profile] : m_CommandRegistry)
            {
                if (name.size() >= current_word.size() &&
                    std::equal(current_word.begin(), current_word.end(), name.begin(), char_iequal))
                {
                    candidates.push_back(name);
                }
            }

            if (candidates.empty())
            {
                addLog("No match for {}", current_word);
            }
            else if (candidates.size() == 1)
            {
                data->DeleteChars((int)(word_start - data->Buf), (int)(word_end - word_start));
                data->InsertChars(data->CursorPos, candidates[0].data());
                data->InsertChars(data->CursorPos, " ");
            }
            else
            {
                size_t match_len = current_word.size();
                while (match_len < candidates[0].size())
                {
                    const char expected_char = (char)toupper(candidates[0][match_len]);
                    bool is_match = std::all_of(candidates.begin() + 1, candidates.end(), [&](std::string_view cand) {
                        return match_len < cand.size() && (char)toupper(cand[match_len]) == expected_char;
                        });

                    if (!is_match) break;

                    match_len++;
                }

                if (match_len > 0)
                {
                    data->DeleteChars((int)(word_start - data->Buf), (int)(word_end - word_start));
                    data->InsertChars(data->CursorPos, candidates[0].data(), candidates[0].data() + match_len);
                }
                addLog("Commands:\n");
                for (auto const& cand : candidates)
                    addLog("{}", cand);
            }
            break;
        }
        case ImGuiInputTextFlags_CallbackHistory:
        {
            const int prev_history_pos = m_HistoryPos;
            if (data->EventKey == ImGuiKey_UpArrow)
            {
                if (m_HistoryPos == -1)
                    m_HistoryPos = (int)m_History.size() - 1;
                else if (m_HistoryPos > 0)
                    m_HistoryPos--;
            }
            else if (data->EventKey == ImGuiKey_DownArrow)
            {
                if (m_HistoryPos != -1)
                    if (++m_HistoryPos >= (int)m_History.size())
                        m_HistoryPos = -1;
            }

            if (prev_history_pos != m_HistoryPos)
            {
                const char* historyStr = (m_HistoryPos >= 0) ? m_History[m_HistoryPos].c_str() : "";
                data->DeleteChars(0, data->BufTextLen);
                data->InsertChars(0, historyStr);
            }
            break;
        }
        }
        return 0;
    }

}