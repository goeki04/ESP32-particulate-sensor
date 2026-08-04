# ESP32 Particulate Sensor

Real-time visualisation of particulate-matter sensor data from a smart home
setup. An ESP32 with a Bosch BMV080 sensor reports through Home Assistant /
ESPHome; a custom C++20 OpenGL engine renders the readings as a GPU particle
system, where particle size maps to the PM class.

> The repository name is historical - the bulk of the code is the **Andromeda**
> engine, not ESP32 firmware.

## Repository layout

| Path | What it is |
| :--- | :--- |
| [`andromeda/`](andromeda/) | The C++20 / OpenGL engine. See [its README](andromeda/README.md) for architecture and build details. |
| [`andromeda_launcher/`](andromeda_launcher/) | Flutter desktop launcher that fetches and starts engine releases. |
| [`.github/workflows/`](.github/workflows/) | CI: Windows + Linux engine builds, launcher build. |
| `Doxyfile` | Doxygen config. Output goes to `docs/` and is **not** committed - run `doxygen` to regenerate. |

## Quick start

**Prerequisites:** CMake ≥ 3.23, Ninja, Conan 2, and a C++20 compiler
(MSVC 2022 or Clang 18+).

```bash
pip install conan
conan profile detect --force
```

**Windows:**

```powershell
cd andromeda; .\build.ps1
```

**Linux:**

```bash
cd andromeda && ./build.sh
```

Then configure and build:

```bash
cmake --preset conan-release && cmake --build --preset conan-release
```

### Tests

```bash
ctest --preset conan-release --output-on-failure
```

The subsystem start-up tests need a window and a live GL context, so they are
excluded from the headless suite. To build and run them locally:

```bash
cmake --preset conan-release -DANDROMEDA_BUILD_GL_TESTS=ON
```

## Configuration

The engine reads Home Assistant credentials from `andromeda/secrets.json`,
which is gitignored. Copy the template and fill in your own values:

```bash
cp andromeda/secrets.example.json andromeda/secrets.json
```

Create the token in Home Assistant under **Profile → Security → Long-lived
access tokens**. Never commit this file.

## Documentation

API documentation is generated, not committed:

```bash
doxygen Doxyfile
```

Output lands in `docs/html/index.html`.

## Status

The `ANDROMEDA_GRAPHICS_API` switch currently only selects the backend for the
`renderer_rhi` module. The `renderer` and `gui` modules still issue raw OpenGL
calls, so Vulkan and DirectX are reserved names rather than working backends.
The Linux build is wired into CI but is not green yet and does not block merges.
