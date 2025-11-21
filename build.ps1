if (-not (Test-Path "conan")) {
    New-Item -ItemType Directory -Path "conan" | Out-Null
}
$toolchainFile = Join-Path $PSScriptRoot "conan/build/generators/conan_toolchain.cmake"
if (-not (Test-Path "cmake")) {
    New-Item -ItemType Directory -Path "cmake" | Out-Null
}

conan install . `
    -s build_type=Debug `
    -of conan/ `
    --build=missing

Set-Location "cmake"
cmake .. `
    -DCMAKE_BUILD_TYPE=Debug `
    -"DCMAKE_TOOLCHAIN_FILE=$toolchainFile"

Read-Host "Press enter to close."