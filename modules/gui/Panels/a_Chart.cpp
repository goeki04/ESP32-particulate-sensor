#include "a_Chart.hpp"
#include "a_EditorContext.hpp"
#include <implot.h>
#include "a_event_manager.hpp"
namespace Andromeda::Gui {
    void Chart::initPanel(EditorContext& ctx)
    {
        m_SelectedEntity.registry = ctx.registry;
        EventManager::getInstance().AddEventListener<SceneObjectSelected>([this](const SceneObjectSelected& event) {
            m_SelectedEntity.id = event.entity;
            });
    }

    void Chart::onGuiRender(EditorContext& ctx) {
            ImGui::Begin("Live Performance Chart", nullptr);

            static ScrollingBuffer sdata(1000);
            static float t = 0;

            t += ImGui::GetIO().DeltaTime;

            float current_value = sinf(t * 2.0f);
            sdata.AddPoint(t, current_value);
            static ImPlotAxisFlags flags = ImPlotAxisFlags_NoTickLabels;

            ImPlot::SetNextAxesToFit();

            if (ImPlot::BeginPlot("##Scrolling", ImVec2(-1, 300))) {
                ImPlot::SetupAxes("Zeit", "Wert", flags, flags);

                ImPlot::PlotLine("Live Signal", &sdata.Data[0].x, &sdata.Data[0].y,
                    sdata.Data.size(), 0, sdata.Offset, 2 * sizeof(float));

                ImPlot::EndPlot();
            }

            ImGui::End();
    }
}