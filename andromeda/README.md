# Smart Home Visual Engine (C++20 / OpenGL)

This project is a high-performance, data-oriented visualization engine for smart home environments. Instead of relying on traditional, often sluggish web frontends, this application leverages an **Entity Component System (ECS)** and **hardware-near graphics APIs** to display sensor and actuator data (e.g., Home Assistant, ESPHome) in real-time.

---

## Key Features

*   **Networking:** Home Assistant over a Boost.Beast WebSocket (authenticated,
    on its own thread) plus httplib for REST calls. An ESPHome native API client
    and Matter are planned but not implemented.
*   **Data-Oriented Design (DOD):** Implements an ECS to strictly separate data (Components) from logic (Systems), optimized for CPU cache efficiency.
*   **Renderer RHI:** Custom renderer hardware interface. Note that only OpenGL
    is implemented, and `renderer`/`gui` still issue raw `gl*` calls, so the
    abstraction is not complete yet.
*   **GPU particles:** Compute-shader driven emitter. Shader reflection generates
    the matching C++ uniform structs from SPIR-V during the build.
*   **Modern C++ Toolchain:** Full utilization of C++20 features, SDL3 for window management, and Conan for clean dependency handling.

---

## Requirements

*   **Compiler:** C++20 compatible (MSVC 2022 recommended, GCC 11+, Clang 13+).
*   **Graphics:** OpenGL 4.5 support (architecture prepared for Vulkan backend).
*   **Package Manager:** [Conan](https://conan.io/) for automated library management.
*   **Build System:** [CMake](https://cmake.org/) ≥ 3.23.

---

## Toolchain Setup

1.  ** Installation: ** Install CMake, Ninja-Build, and MSVC (with C++20 workload).
2.  ** Conan Configuration: **
    ```powershell
    pip install conan
    conan profile detect --force
    ```
3.  ** Build Process:**
    *   Run the `build.ps1` script to install dependencies through Conan.
    *   Then `cmake --preset conan-release` and `cmake --build --preset conan-release`.
    *   Both profiles use the Ninja generator, so no `.sln` is produced.
4.  ** Coding Standard:**
    *   **Important:** All code is written in **English** as per project guidelines.

---

## Architecture Overview

The project strictly separates **Frontend** (Rendering/UI) and **Backend** (Logic/Network):

| Layer | Technology | Responsibility |
| :--- | :--- | :--- |
| **Graphics** | OpenGL / ImGui | Visualization & Dashboard UI |
| **Logic** | ECS (Systems) | Transforming sensor data into graphical components |
| **Networking** | Boost.Beast WebSocket / httplib | Thread-safe communication with Home Assistant |

---

## Why this Approach?

In modern software engineering (keyword: *Data-Oriented Design*), the hardware is the reality. This engine avoids unnecessary OOP hierarchies to minimize the latency between sensor updates and visual representation. It combines the raw power of C++ with the flexibility of modern smart home interfaces.

---
*This project serves as a deep-dive into high-performance systems programming and asynchronous I/O during my first year of apprenticeship.*
