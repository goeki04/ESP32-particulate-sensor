#pragma once
#include <vector>
#include "imgui.h"
#include "a_EditorPanel.hpp"
#include "a_registry.hpp"
namespace Andromeda::Gui {
    struct ScrollingBuffer {
        int MaxSize;
        int Offset;
        std::vector<ImVec2> Data;
        ScrollingBuffer(int max_size = 500) {
            MaxSize = max_size;
            Offset = 0;
            Data.reserve(MaxSize);
        }
        void AddPoint(float x, float y) {
            if (Data.size() < MaxSize)
                Data.push_back(ImVec2(x, y));
            else {
                Data[Offset] = ImVec2(x, y);
                Offset = (Offset + 1) % MaxSize;
            }
        }
    };
    struct EditorContext;
	class Chart : public EditorPanel {
    public:
        explicit Chart(const char* name)
            : EditorPanel(name)
        {
            m_IsOpen = true;
        }
        void initPanel(EditorContext& ctx) override;
        void onGuiRender(EditorContext& ctx) override;
    private:
        ECS::EntityHandle m_SelectedEntity;
	};
}