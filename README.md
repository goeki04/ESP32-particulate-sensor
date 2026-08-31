# Andromeda

A C++20 / OpenGL engine for visualising smart home sensor data. The goal is to
show particulate matter readings from a Bosch BMV080 as a GPU particle system
instead of yet another web dashboard.

This started as an apprenticeship project and is very much still in progress.

## NOTE:
The andromeda launcher is almost fully vibe coded because its just an app launcher.
The REAL andromeda project has been created fully by me with ai assisting in code documentation
and unit tests / refactoring.
The architecture and actual implementation is all done by myself

## Where it actually stands

Working:

* GPU particle system driven by a compute shader (HLSL, compiled to SPIR-V)
* Shader reflection that generates C++ structs from SPIR-V at build time
* WebSocket connection to Home Assistant over Boost.Beast, with auth
* ECS registry, scene serialisation, ImGui editor with viewport and panels
* Flutter launcher that lists commits and downloads engine builds

Not there yet:

* The PM1 / PM2.5 / PM10 values arrive from Home Assistant but are only written
  to the log. Nothing feeds them into the particle system, so what you see on
  screen is still a synthetic emitter.
* `ANDROMEDA_GRAPHICS_API` only picks the backend for the `renderer_rhi` module.
  `renderer` and `gui` still make raw `gl*` calls, so Vulkan and DirectX are
  placeholder names, not working backends.
* No ESPHome native API and no Matter support. Home Assistant over
  WebSocket + REST is the only transport that exists.
* The Linux build is wired into CI but not green yet, so it does not block.

## Building

You need CMake 3.23+, Ninja, Conan 2 and either MSVC 2022 or Clang 18+.

```
pip install conan
conan profile detect --force
```

On Windows run `andromeda\build.ps1`, on Linux `andromeda/build.sh`. Both just
install dependencies through Conan. After that:

```
cmake --preset conan-release
cmake --build --preset conan-release
```

Tests:

```
ctest --preset conan-release --output-on-failure
```

The subsystem start-up tests need a real window and GL context, so they are not
part of that run. Build them with `-DANDROMEDA_BUILD_GL_TESTS=ON` if you want
them locally.

## Home Assistant setup

The engine reads its credentials from `andromeda/secrets.json`, which is
gitignored. Copy `andromeda/secrets.example.json` over it and put in your own
URL and token. The token comes from Home Assistant under Profile → Security →
Long-lived access tokens.

## Layout

`andromeda/` is the engine. `andromeda_launcher/` is a Flutter desktop app that
fetches releases and starts them. Everything else is build and CI plumbing.

Docs are generated, not committed. Run `doxygen Doxyfile` and open
`docs/html/index.html`.

## License

Apache 2.0, see [LICENSE](LICENSE). Third-party components keep their own terms,
listed under `andromeda/third_party_licenses/`.
