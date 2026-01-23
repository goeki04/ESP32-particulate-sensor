from conan import ConanFile
from conan.tools.cmake import cmake_layout
import requests
class CmakeTest(ConanFile):
    generators = ("CMakeToolchain", "CMakeDeps")
    settings = ("os","build_type", "arch", "compiler")
    def requirements(self):
        self.requires("glm/1.0.1")
        self.requires("sdl/3.2.20")
        self.requires("imgui/1.92.5")
        self.requires("glew/2.2.0")
        self.requires("assimp/6.0.2")
        self.requires("paho-mqtt-cpp/1.5.3")
        self.requires("stb/cci.20230920")
        self.requires("paho-mqtt-c/1.3.13")
    def layout(self):
        cmake_layout(self)