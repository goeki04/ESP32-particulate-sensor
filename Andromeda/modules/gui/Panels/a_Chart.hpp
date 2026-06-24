#pragma once

/**
 * @file a_Chart.hpp
 * @brief Editor panel that plots live time-series data (e.g. sensor readings) using ImPlot.
 */

#include <vector>
#include "imgui.h"
#include "a_EditorPanel.hpp"
#include "a_registry.hpp"
namespace Andromeda::Gui {
    /**
     * @struct ScrollingBuffer
     * @brief Fixed-capacity ring buffer of 2D points for real-time scrolling plots.
     *
     * @details Once @c MaxSize points have been added, new points overwrite the oldest
     *          ones in a circular fashion, so the buffer always holds the most recent
     *          window of samples without reallocating. Stores (x, y) pairs as @c ImVec2,
     *          where x is typically a timestamp and y the measured value.
     */
    struct ScrollingBuffer {
        int MaxSize;              ///< Maximum number of points retained before overwriting begins.
        int Offset;               ///< Index of the oldest element / next write position once full.
        std::vector<ImVec2> Data; ///< Backing storage of (x, y) sample points.

        /**
         * @brief Constructs a scrolling buffer with the given capacity.
         * @param max_size Maximum number of points to retain (default 500).
         */
        ScrollingBuffer(int max_size = 500) {
            MaxSize = max_size;
            Offset = 0;
            Data.reserve(MaxSize);
        }

        /**
         * @brief Appends a new sample, overwriting the oldest one if the buffer is full.
         * @param x The x-coordinate of the sample (e.g. time).
         * @param y The y-coordinate of the sample (e.g. measured value).
         */
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

    /**
     * @class Chart
     * @brief Panel that visualizes the time-varying data of the selected entity as a live graph.
     */
	class Chart : public EditorPanel {
    public:
        /**
         * @brief Constructs the Chart panel.
         * @param name The display title of the panel window.
         */
        explicit Chart(const char* name)
            : EditorPanel(name)
        {
            m_IsOpen = true;
        }

        /** @copydoc EditorPanel::initPanel */
        void initPanel(EditorContext& ctx) override;
        /** @copydoc EditorPanel::onGuiRender */
        void onGuiRender(EditorContext& ctx) override;
    private:
        ECS::EntityHandle m_SelectedEntity; ///< The entity whose data is currently being plotted.
	};
}