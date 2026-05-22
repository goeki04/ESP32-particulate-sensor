#pragma once
#include <glm/glm.hpp>

namespace Andromeda {

    // =========================================================================
    // 1. Globale Kamera- & Objekt-Strukturen
    // =========================================================================

    // Verwendet in: 
    // - color.vert (Binding 0)
    // - mask.vert  (Binding 0)
    // - skybox.vert (Binding 0) -> via Alias
    // - unlit.vert  (Binding 0) -> via Alias
    // - equirect.vert (Binding 0) -> via Alias
    struct CameraBuffer {
        glm::mat4 viewMatrix; // 64 Bytes
        glm::mat4 projMatrix; // 64 Bytes
    }; // Gesamtgröße: 128 Bytes
    static_assert(sizeof(CameraBuffer) % 16 == 0, "Andromeda Alignment Error: CameraBuffer size must be a multiple of 16 bytes!");

    // Verwendet in:
    // - color.vert (Binding 1)
    // - mask.vert  (Binding 1)
    // - unlit.vert (Binding 1) -> via Alias
    struct ObjectBuffer {
        glm::mat4 model;     // 64 Bytes
    }; // Gesamtgröße: 64 Bytes
    static_assert(sizeof(ObjectBuffer) % 16 == 0, "Andromeda Alignment Error: ObjectBuffer size must be a multiple of 16 bytes!");

    // =========================================================================
    // 2. Einfache Einfärbung (color.frag)
    // =========================================================================

    // Verwendet in color.frag (Binding 2)
    struct ColorBuffer {
        alignas(16) glm::vec3 aColor; // 12 Bytes
        float padding;                //  4 Bytes -> füllt die std140 vec3-Lücke auf
    }; // Gesamtgröße: 16 Bytes
    static_assert(sizeof(ColorBuffer) % 16 == 0, "Andromeda Alignment Error: ColorBuffer size must be a multiple of 16 bytes!");

    // =========================================================================
    // 3. Unendliches Grid (grid.vert & grid.frag)
    // =========================================================================

    // Verwendet in grid.vert (Binding 0)
    struct GridBuffer {
        glm::mat4 viewMatrix;         // 64 Bytes
        glm::mat4 projMatrix;         // 64 Bytes
        alignas(16) glm::vec3 camPos; // 12 Bytes
        float gGridSize;              //  4 Bytes -> füllt auf 16 Bytes auf
    }; // Gesamtgröße: 144 Bytes
    static_assert(sizeof(GridBuffer) % 16 == 0, "Andromeda Alignment Error: GridBuffer size must be a multiple of 16 bytes!");

    // Verwendet in grid.frag (Binding 1)
    // HIER WAREN SIE VERLOREN GEGANGEN – JETZT WIEDER DA:
    struct GridParamsBuffer {
        float gGridMinPixelsBetweenCells; //  4 Bytes
        float gGridCellSize;              //  4 Bytes
        // 8 Bytes implizites Padding durch das nachfolgende alignas(16)

        alignas(16) glm::vec4 gGridColorThin;  // 16 Bytes
        alignas(16) glm::vec4 gGridColorThick; // 16 Bytes

        alignas(16) glm::vec3 camPos;          // 12 Bytes
        float gGridSize;                       //  4 Bytes -> füllt auf 16 Bytes auf
    }; // Gesamtgröße: 64 Bytes
    static_assert(sizeof(GridParamsBuffer) % 16 == 0, "Andromeda Alignment Error: GridParamsBuffer size must be a multiple of 16 bytes!");

    // =========================================================================
    // 4. Selektions-Kanten (outline.frag)
    // =========================================================================

    // Verwendet in outline.frag (Binding 0)
    struct OutlineParamsBuffer {
        glm::vec2 texelSize; //  8 Bytes
        float padding[2];    //  8 Bytes -> füllt die Gesamtstruktur auf 16 Bytes auf
    }; // Gesamtgröße: 16 Bytes
    static_assert(sizeof(OutlineParamsBuffer) % 16 == 0, "Andromeda Alignment Error: OutlineParamsBuffer size must be a multiple of 16 bytes!");

    // =========================================================================
    // 5. Shader-Spezifische Type-Aliase (Gleiches Layout, andere Semantik)
    // =========================================================================
    using SkyboxBuffer = CameraBuffer; // Für skybox.vert (Binding 0)
    using UnlitCameraBuffer = CameraBuffer; // Für unlit.vert  (Binding 0)
    using BakeBuffer = CameraBuffer; // Für equirect.vert (Binding 0)

    using UnlitObjectBuffer = ObjectBuffer; // Für unlit.vert (Binding 1)

}