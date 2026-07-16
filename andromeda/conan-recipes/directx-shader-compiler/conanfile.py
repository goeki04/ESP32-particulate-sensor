from conan import ConanFile
from conan.tools.files import get, copy
from conan.errors import ConanInvalidConfiguration
import os

class DirectXShaderCompiler(ConanFile):
    name = "directx-shader-compiler"
    version = "1.9.2602"

    settings = "os", "arch"
    package_type = "application"

    description = "DirectX Shader Compiler (DXC) is a compiler for HLSL."
    homepage = "https://github.com/microsoft/DirectXShaderCompiler"
    license = "LLVM-Exception"

    # SHA256 of the prebuilt Windows release asset for this version, fetched
    # from the GitHub release metadata. Ensures the download can't silently
    # be swapped/corrupted.
    _windows_sha256 = "a1e89031421cf3c1fca6627766ab3020ca4f962ac7e2caa7fab2b33a8436151e"

    def validate(self):
        if self.settings.os != "Windows":
            raise ConanInvalidConfiguration("directx-shader-compiler currently only packages the Windows DXC binaries.")

    def build(self):
        url = f"https://github.com/microsoft/DirectXShaderCompiler/releases/download/v{self.version}/dxc_2026_02_20.zip"
        # get() downloads the zip and extracts it directly into the build folder
        get(self, url, sha256=self._windows_sha256)

    def package(self):
        # The release zip ships per-architecture subfolders (bin/x64, bin/arm64, ...).
        # Flatten the one matching our arch into package_folder/bin so that
        # find_program()/CMAKE_PROGRAM_PATH can locate dxc.exe directly.
        arch_folder = "arm64" if self.settings.arch == "armv8" else "x64"
        copy(self, "*", src=os.path.join(self.build_folder, "bin", arch_folder),
             dst=os.path.join(self.package_folder, "bin"))
        copy(self, "*", src=os.path.join(self.build_folder, "lib", arch_folder),
             dst=os.path.join(self.package_folder, "lib"))
        copy(self, "*", src=os.path.join(self.build_folder, "inc"),
             dst=os.path.join(self.package_folder, "include"))
        copy(self, "LICENSE*", src=self.build_folder,
             dst=os.path.join(self.package_folder, "licenses"))

    def package_info(self):
        bin_path = os.path.join(self.package_folder, "bin")
        self.buildenv_info.prepend_path("PATH", bin_path)