#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstring>
#include <unordered_map>
#include <sstream>
#include <filesystem>
#include "spirv_reflect.h"
#include "shader_comment_parser.hpp"
#include <iomanip>
// Macro Stringification Helpers
#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

namespace fs = std::filesystem;

struct StructDefinition {
    uint32_t size = 0;
    std::string code = "";
};

std::vector<char> ReadSpvFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::ate | std::ios::binary);
    if (!file.is_open()) throw std::runtime_error("Failed to open " + filename);

    size_t fileSize = (size_t)file.tellg();
    std::vector<char> buffer(fileSize);
    file.seekg(0);
    file.read(buffer.data(), fileSize);
    file.close();
    return buffer;
}

std::string GetCppType(const SpvReflectTypeDescription* type) {
    if (type->type_flags & SPV_REFLECT_TYPE_FLAG_MATRIX) {
        if (type->traits.numeric.matrix.column_count == 4 && type->traits.numeric.matrix.row_count == 4) return "mat4";
        if (type->traits.numeric.matrix.column_count == 3 && type->traits.numeric.matrix.row_count == 3) return "mat3";
    }
    if (type->type_flags & SPV_REFLECT_TYPE_FLAG_VECTOR) {
        if (type->traits.numeric.vector.component_count == 4) return "vec4";
        if (type->traits.numeric.vector.component_count == 3) return "vec3";
        if (type->traits.numeric.vector.component_count == 2) return "vec2";
    }
    if (type->type_flags & SPV_REFLECT_TYPE_FLAG_FLOAT) return "float";
    if (type->type_flags & SPV_REFLECT_TYPE_FLAG_INT) return "i32";

    return "UNKNOWN_TYPE";
}

std::string GetShaderDataTypeEnum(const std::string& cppType) {
    if (cppType == "mat4")  return "ShaderDataType::Mat4";
    if (cppType == "mat3")  return "ShaderDataType::Mat3";
    if (cppType == "vec4")  return "ShaderDataType::Vec4";
    if (cppType == "vec3")  return "ShaderDataType::Vec3";
    if (cppType == "vec2")  return "ShaderDataType::Vec2";
    if (cppType == "float") return "ShaderDataType::Float";
    if (cppType == "i32")   return "ShaderDataType::Int";
    return "ShaderDataType::Unknown";
}

std::string FindShaderFile(const std::string& baseDir, const std::string& targetFilename) {
    if (!fs::exists(baseDir)) return "";

    for (const auto& entry : fs::recursive_directory_iterator(baseDir)) {
        if (entry.is_regular_file() && entry.path().filename().string() == targetFilename) {
            return entry.path().string();
        }
    }
    return ""; // Not found
}

/**
 * Emits the member declarations + reflect() visitor body for any block-like variable.
 * Shared by top-level uniform blocks (e.g. EmitterSettings) and named struct types found
 * nested inside storage-buffer arrays (e.g. Particle inside `Particle particles[]`), since
 * both just need their `.members` walked the same way.
 */
std::string BuildStructBody(const SpvReflectBlockVariable& block,
    const std::unordered_map<std::string, PropertyUiMetadata>& uiMetadata) {
    std::ostringstream structCode;
    std::ostringstream visitorCode;

    visitorCode << "    // Automatically generated reflection visitor for ImGui and Serialization\n";
    visitorCode << "    template<typename F>\n";
    visitorCode << "    void reflect(F&& visitor) {\n";

    for (uint32_t m = 0; m < block.member_count; m++) {
        SpvReflectBlockVariable& member = block.members[m];
        std::string cppType = GetCppType(member.type_description);
        bool isArray = false;
        uint32_t arraySize = 0;

        if (member.type_description->traits.array.dims_count > 0) {
            isArray = true;
            arraySize = member.type_description->traits.array.dims[0];
        }

        structCode << "    // Offset: " << member.offset << "\n";

        bool needsAlignment = (cppType.find("vec") != std::string::npos ||
            cppType.find("mat") != std::string::npos ||
            isArray);

        if (needsAlignment) {
            structCode << "    alignas(16) ";
        }
        else {
            structCode << "    ";
        }

        if (isArray) {
            structCode << cppType << " " << member.name << "[" << arraySize << "];\n";
        }
        else {
            structCode << cppType << " " << member.name << ";\n";

            std::string enumType = GetShaderDataTypeEnum(cppType);

            float minVal = 0.0f;
            float maxVal = 0.0f;
            std::string widgetHint = "Default";
            if (uiMetadata.find(member.name) != uiMetadata.end()) {
                const auto& meta = uiMetadata.at(member.name);
                if (meta.widgetType == "Slider") {
                    minVal = meta.minBound;
                    maxVal = meta.maxBound;
                    widgetHint = "Slider";
                }
                else if (meta.widgetType == "Color") {
                    widgetHint = "Color";
                }
            }

            if (widgetHint == "Color") {
                visitorCode << "        visitor(\"" << member.name << "\", " << member.name << ", " << enumType
                    << ", 0.0f, 0.0f, \"Color\");\n";
            }
            else if (widgetHint == "Slider" && minVal != maxVal) {
                visitorCode << "        visitor(\"" << member.name << "\", " << member.name << ", " << enumType << ", "
                    << std::fixed << std::setprecision(2) << minVal << "f, "
                    << std::fixed << std::setprecision(2) << maxVal << "f, \"Slider\");\n";
            }
            else {
                visitorCode << "        visitor(\"" << member.name << "\", " << member.name << ", " << enumType
                    << ", 0.0f, 0.0f, \"Default\");\n";
            }
        }
    }

    visitorCode << "    }\n";
    structCode << "\n" << visitorCode.str();
    return structCode.str();
}

/**
 * Registers a fully-formed C++ struct (declaration + reflect() visitor + static_assert) for
 * `structName` in `targetStructs`, unless an equal-or-larger definition is already cached
 * (mirrors the old dedup rule: the same block name can appear in several shaders, keep the
 * biggest one seen).
 */
void EmitStruct(std::unordered_map<std::string, StructDefinition>& targetStructs,
    const std::string& structName,
    const SpvReflectBlockVariable& block,
    uint32_t size,
    const std::string& sourceFile,
    const std::unordered_map<std::string, PropertyUiMetadata>& uiMetadata) {

    auto it = targetStructs.find(structName);
    if (it != targetStructs.end() && it->second.size >= size) {
        return;
    }

    std::ostringstream out;
    out << "// Generated from: " << sourceFile << "\n";
    out << "struct alignas(16) " << structName << " {\n";
    out << BuildStructBody(block, uiMetadata);
    out << "}; // Total Size: " << size << " Bytes\n";
    out << "static_assert(sizeof(" << structName << ") % 16 == 0, \"Alignment Error!\");\n\n";

    targetStructs[structName] = { size, out.str() };
}

int main(int argc, char** argv) {
    if (argc < 4) {
        std::cerr << "Usage: AndromedaReflector <output.hpp> <output_compute.hpp> <input1.spv> [input2.spv...]\n";
        return 1;
    }

    std::string outputPath = argv[1];
    std::string outputComputePath = argv[2];

#ifndef SHADER_PATH
#error "SHADER_PATH macro is missing! Check your CMake target_compile_definitions."
#endif

    std::string baseSourceDir = TOSTRING(SHADER_PATH);

    // Cleanup in case CMake sent double quotes
    if (!baseSourceDir.empty() && baseSourceDir.front() == '"') baseSourceDir.erase(0, 1);
    if (!baseSourceDir.empty() && baseSourceDir.back() == '"') baseSourceDir.pop_back();

    std::ofstream outFile(outputPath, std::ios::out);
    if (!outFile.is_open()) {
        std::cerr << "Failed to open output file.\n";
        return 1;
    }

    std::ofstream outComputeFile(outputComputePath, std::ios::out);
    if (!outComputeFile.is_open()) {
        std::cerr << "Failed to open compute output file.\n";
        return 1;
    }

    outFile << "#pragma once\n";
    outFile << "#include \"a_primitives.hpp\"\n";
    outFile << "#include \"a_IGraphicsContext.hpp\" // Requires ShaderDataType\n\n";
    outFile << "namespace Andromeda::Generated {\n\n";

    outComputeFile << "#pragma once\n";
    outComputeFile << "#include \"a_primitives.hpp\"\n";
    outComputeFile << "#include \"a_IGraphicsContext.hpp\" // Requires ShaderDataType\n\n";
    outComputeFile << "namespace Andromeda::Generated::Compute {\n\n";

    // Uniform-block (and SSBO element) structs from vertex/fragment (and any other non-compute) shaders.
    std::unordered_map<std::string, StructDefinition> generatedStructs;
    // Same, but for compute shaders - written to their own namespace/file so they don't
    // collide with graphics-pipeline structs of the same name.
    std::unordered_map<std::string, StructDefinition> generatedComputeStructs;
    std::ofstream initLog("reflector_debug.log", std::ios::trunc);
    if (initLog.is_open()) {
        initLog << "--- Starting new Reflector run ---\n";
        initLog.close();
    }
    for (int i = 3; i < argc; ++i) {
        std::string inputPath = argv[i];

        std::string filename = fs::path(inputPath).filename().string();

        size_t spvExtPos = filename.rfind(".spv");
        if (spvExtPos != std::string::npos) {
            filename = filename.substr(0, spvExtPos);
        }

        std::string glslPath = FindShaderFile(baseSourceDir, filename);

        if (glslPath.empty()) {
            std::ofstream logFile("reflector_debug.log", std::ios::app);
            if (logFile.is_open()) {
                logFile << "[ERROR] File '" << filename << "' not found anywhere in: " << baseSourceDir << "\n";
                logFile.close();
            }
            continue;
        }

        std::unordered_map<std::string, PropertyUiMetadata> uiMetadata = ParseGlslAnnotations(glslPath);

        std::ofstream logFile("reflector_debug.log", std::ios::app);
        if (logFile.is_open()) {
            logFile << "[DEBUG] trying to parse glsl files: " << glslPath << "\n";
            logFile << "[DEBUG] found Annotations: " << uiMetadata.size() << "\n";
            for (const auto& pair : uiMetadata) {
                logFile << "  -> Variable: '" << pair.first << "' | Widget: "
                    << pair.second.widgetType << " | Min: " << pair.second.minBound
                    << " | Max: " << pair.second.maxBound << "\n";
            }
            logFile << "--------------------------------------------------\n";
            logFile.close();
        }
        else {
            std::cerr << "[ERROR] Could not open reflector_debug.log!\n";
        }

        std::vector<char> spvCode;
        try {
            spvCode = ReadSpvFile(inputPath);
        }
        catch (const std::exception& e) {
            std::cerr << "Warning: Could not read " << inputPath << ". Skipping.\n";
            continue;
        }

        SpvReflectShaderModule module;
        if (spvReflectCreateShaderModule(spvCode.size(), spvCode.data(), &module) != SPV_REFLECT_RESULT_SUCCESS) {
            std::cerr << "Warning: Failed to parse SPIR-V for " << inputPath << ". Skipping.\n";
            continue;
        }

        // Compute shaders get their structs generated into Andromeda::Generated::Compute
        // instead of Andromeda::Generated, so they land in their own file and never collide
        // with a graphics-pipeline uniform block that happens to share a name.
        bool isCompute = (module.shader_stage & SPV_REFLECT_SHADER_STAGE_COMPUTE_BIT) != 0;
        std::unordered_map<std::string, StructDefinition>& targetStructs =
            isCompute ? generatedComputeStructs : generatedStructs;

        uint32_t bindingCount = 0;
        spvReflectEnumerateDescriptorBindings(&module, &bindingCount, nullptr);
        std::vector<SpvReflectDescriptorBinding*> bindings(bindingCount);
        spvReflectEnumerateDescriptorBindings(&module, &bindingCount, bindings.data());


        for (uint32_t b = 0; b < bindingCount; b++) {
            SpvReflectDescriptorBinding* binding = bindings[b];

            if (binding->descriptor_type == SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER) {
                std::string structName = "";
                if (binding->name && strlen(binding->name) > 0) structName = binding->name;
                else if (binding->type_description && binding->type_description->type_name) structName = binding->type_description->type_name;
                else structName = "UnknownBuffer_Binding" + std::to_string(binding->binding);

                EmitStruct(targetStructs, structName, binding->block, binding->block.size, inputPath, uiMetadata);
            }
            else if (binding->descriptor_type == SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_BUFFER) {
                // SSBOs are usually unbounded (e.g. `Particle particles[]`), so there is no
                // fixed-size struct to generate for the block itself. What the CPU side does
                // need is the element type's exact layout/size (to allocate/stride the buffer),
                // so walk the block's direct members and generate a struct for any named
                // struct type found among them (e.g. `Particle`).
                for (uint32_t m = 0; m < binding->block.member_count; m++) {
                    SpvReflectBlockVariable& member = binding->block.members[m];
                    SpvReflectTypeDescription* memberType = member.type_description;
                    if (!memberType) continue;
                    if (!(memberType->type_flags & SPV_REFLECT_TYPE_FLAG_STRUCT)) continue;
                    if (member.member_count == 0) continue;

                    std::string elemStructName;
                    if (memberType->type_name && strlen(memberType->type_name) > 0) elemStructName = memberType->type_name;
                    else if (member.name && strlen(member.name) > 0) elemStructName = member.name;
                    else continue;

                    // For arrays, .array.stride is the per-element byte size (what the CPU
                    // needs for sizeof/allocation purposes); for a plain (non-array) nested
                    // struct member, fall back to its own reflected size.
                    uint32_t elemSize = member.array.dims_count > 0 ? member.array.stride : member.size;
                    if (elemSize == 0) elemSize = member.padded_size;

                    EmitStruct(targetStructs, elemStructName, member, elemSize, inputPath, uiMetadata);
                }
            }
        }
        spvReflectDestroyShaderModule(&module);
    }
    for (const auto& pair : generatedStructs) {
        outFile << pair.second.code;
    }
    for (const auto& pair : generatedComputeStructs) {
        outComputeFile << pair.second.code;
    }

    outFile << "} // namespace Andromeda::Generated\n";
    outFile.close();

    outComputeFile << "} // namespace Andromeda::Generated::Compute\n";
    outComputeFile.close();
    return 0;
}
