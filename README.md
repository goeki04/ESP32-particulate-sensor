# Smart Home Visual Engine (C++20 / OpenGL)

This project is a high-performance, data-oriented visualization engine for smart home environments. Instead of relying on traditional, often sluggish web frontends, this application leverages an **Entity Component System (ECS)** and **hardware-near graphics APIs** to display sensor and actuator data (e.g., Home Assistant, ESPHome) in real-time.

---

## Key Features

*   **Hybrid Networking Architecture:** 
    *   **MQTT (Paho):** Asynchronous reception of real-time sensor data (e.g., Bosch sensors) via broker.
    *   **REST API:** Reliable command transmission to Home Assistant actuators.
*   **Data-Oriented Design (DOD):** Implements an ECS to strictly separate data (Components) from logic (Systems), optimized for CPU cache efficiency.
*   **Multi-Threaded Rendering:** Complete decoupling of network I/O from the rendering loop to guarantee stable frame rates, even during blocking REST requests.
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
    *   Run the `build.ps1` script to install dependencies and generate project files.
    *   Open the generated `.sln` file in the `build` directory.
4.  ** Coding Standard:**
    *   **Important:** All code is written in **English** as per project guidelines.

---

## Architecture Overview

The project strictly separates **Frontend** (Rendering/UI) and **Backend** (Logic/Network):

| Layer | Technology | Responsibility |
| :--- | :--- | :--- |
| **Graphics** | OpenGL / ImGui | Visualization & Dashboard UI |
| **Logic** | ECS (Systems) | Transforming sensor data into graphical components |
| **Networking** |  ESPHome Native API | Thread-safe communication with Smart Home hubs |

---

## Why this Approach?

In modern software engineering (keyword: *Data-Oriented Design*), the hardware is the reality. This engine avoids unnecessary OOP hierarchies to minimize the latency between sensor updates and visual representation. It combines the raw power of C++ with the flexibility of modern smart home interfaces.

---
*This project serves as a deep-dive into high-performance systems programming and asynchronous I/O during my first year of apprenticeship.*
