from conan import ConanFile
from conan.tools.cmake import cmake_layout
import requests
class CmakeTest(ConanFile):
    generators = ("CMakeToolchain", "CMakeDeps")
    settings = ("os","build_type", "arch", "compiler")
    def requirements(self):
        self.requires("glm/1.0.1")
        self.requires("sdl/3.2.20")
        self.requires("stb/cci.20240531")
        self.requires("imgui/1.92.4")
        self.requires("glew/2.2.0")
    def layout(self):
        cmake_layout(self)