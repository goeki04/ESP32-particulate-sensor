# ESP32 Particulate Matter Monitor

This project uses an ESP32 paired with a particulate matter (PM) sensor to measure air quality (PM2.5 and PM10) and display the readings in real-time. It is ideal for DIY air quality monitoring or smart home applications.

## Features

- Real-time measurement of PM1, PM2.5 and PM10
- Display measurements on a connected screen or via serial output
- Easy integration with ESP32 projects
- Cross-platform build using CMake and Conan

## Requirements

To build and run this project, you will need:

- **CMake** version 3.23 or higher
- A **C++20 compatible compiler** (MSVC recommended)
- **Conan** package manager for dependency management
- Install ultralytics via python

## Toolchain Setup

1. Install [CMake](https://cmake.org/download/) ≥ 3.23
2. Install [MSVC](https://visualstudio.microsoft.com/) with C++20 support
3. Install [Conan](https://conan.io/) and configure it:
   ```bash
   pip install conan
   conan profile new default --detect
   conan profile update settings.compiler.cppstd=20 default
4. Run the build.ps1 script
5. Open the .sln file in the cmake directory
6. In MSV right-click the ESP32 Project in the solution explorer and set "Set as startup Project"
7. Run the program
