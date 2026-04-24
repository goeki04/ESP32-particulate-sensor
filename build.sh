#!/bin/bash

# Install Debug configuration
conan install . \
  -s compiler.cppstd=20 \
  -s build_type=Debug \
  -of conan/ \
  --build=missing \
  -c tools.cmake.cmaketoolchain:generator=Ninja

# Install Release configuration
conan install . \
  -s compiler.cppstd=20 \
  -s build_type=Release \
  -of conan/ \
  --build=missing \
  -c tools.cmake.cmaketoolchain:generator=Ninja

# Pause before closing
read -p "Press Enter to close"s
