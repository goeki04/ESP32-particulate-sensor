#!/bin/bash
# Build setup for Linux using Clang (profiles/linux for BOTH host -pr and build
# context -pr:b, so the shader tools glslang/spirv-tools also use Clang).
#
# Prerequisites (Ubuntu/Debian):
#   sudo apt install -y clang lld cmake ninja-build git pkg-config python3 python3-pip \
#       libgl1-mesa-dev libglu1-mesa-dev mesa-common-dev xorg-dev libxrandr-dev \
#       libxinerama-dev libxcursor-dev libxi-dev libwayland-dev wayland-protocols \
#       libxkbcommon-dev libdecor-0-dev
#   pip install --user --upgrade conan   # needs CMake >= 3.23
set -e

conan install . -pr profiles/linux -pr:b profiles/linux -s build_type=Debug   -of conan/ --build=missing
conan install . -pr profiles/linux -pr:b profiles/linux -s build_type=Release -of conan/ --build=missing

echo
echo "Conan done. Configure & build with:"
echo "  cmake --preset conan-release && cmake --build --preset conan-release"
