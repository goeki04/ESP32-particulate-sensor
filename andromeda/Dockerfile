# Reproducible build environment for Andromeda (Ubuntu 24.04 + Clang 18).
# This image BUILDS the project. Running the OpenGL GUI from a container
# additionally requires X11/Wayland socket forwarding and GPU passthrough.
#
#   docker build -t andromeda-build .
#   docker run --rm -it andromeda-build            # drops you in /app with the build done
#
# Ubuntu 24.04 ships clang 18 and cmake 3.28 (>= 3.23 required). build.sh
# detects the installed clang major version and overrides the compiler.version
# pinned in profiles/linux, so the two cannot drift apart.
FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y --no-install-recommends \
      clang lld cmake ninja-build git pkg-config ca-certificates \
      python3 python3-pip python3-venv \
      libgl1-mesa-dev libglu1-mesa-dev mesa-common-dev \
      xorg-dev libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev \
      libwayland-dev wayland-protocols libxkbcommon-dev libdecor-0-dev \
    && rm -rf /var/lib/apt/lists/*

# Conan 2 (apt has no reliable conan2 package; install via pip).
RUN pip3 install --no-cache-dir --break-system-packages "conan>=2.0"

WORKDIR /app
COPY . .

# Install dependencies (Debug + Release) and build the Release configuration.
RUN chmod +x build.sh && ./build.sh \
    && cmake --preset conan-release \
    && cmake --build --preset conan-release

CMD ["bash"]
