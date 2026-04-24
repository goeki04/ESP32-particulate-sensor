from conan import ConanFile
from conan.tools.cmake import cmake_layout
import requests
class CmakeTest(ConanFile):
    generators = ("CMakeToolchain", "CMakeDeps")
    settings = ("os","build_type", "arch", "compiler")
    def requirements(self):
        self.requires("glm/1.0.1")
        self.requires("sdl/3.2.20")
        self.requires("imgui/1.92.5-docking", override=True)
        self.requires("glew/2.2.0")
        self.requires("nlohmann_json/3.12.0")
        self.requires("asio/1.36.0")
        self.requires("assimp/6.0.2")
        self.requires("stb/cci.20230920")
        self.requires("protobuf/3.21.12")
        self.requires("imguizmo/cci.20231114")
        self.requires("implot/0.17")
        self.requires("gtest/1.17.0")
    def layout(self):
        cmake_layout(self)