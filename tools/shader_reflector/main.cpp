#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstring>
#include "spirv_reflect.h"
#include <unordered_map>
#include <sstream>
#include "shader_comment_parser.hpp"

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
    if (cppType == "int")   return "ShaderDataType::Int";
    return "ShaderDataType::Unknown";
}

int main(int argc, char** argv) {
    if (argc < 3) {
        std::cerr << "Usage: AndromedaReflector <output.hpp> <input1.spv> [input2.spv...]\n";
        return 1;
    }

    std::string outputPath = argv[1];

    std::ofstream outFile(outputPath, std::ios::out);
    if (!outFile.is_open()) {
        std::cerr << "Failed to open output file.\n";
        return 1;
    }

    outFile << "#pragma once\n";
    outFile << "#include \"a_primitives.hpp\"\n";
    outFile << "#include \"a_IGraphicsContext.hpp\" // Benötigt für ShaderDataType\n\n";
    outFile << "namespace Andromeda::Generated {\n\n";

    struct StructDefinition {
        uint32_t size = 0;
        std::string code = "";
    };

    std::unordered_map<std::string, StructDefinition> generatedStructs;

    for (int i = 2; i < argc; ++i) {
        std::string inputPath = argv[i];
        std::string glslPath = inputPath;
        size_t spvExtPos = glslPath.rfind(".spv");
        if (spvExtPos != std::string::npos) {
            glslPath = glslPath.substr(0, spvExtPos);
        }
        std::unordered_map<std::string, PropertyUiMetadata> uiMetadata = ParseGlslAnnotations(glslPath);
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

                uint32_t currentSize = binding->block.size;

                if (generatedStructs.find(structName) == generatedStructs.end() || generatedStructs[structName].size < currentSize) {

                    std::ostringstream structCode;
                    std::ostringstream visitorCode;

                    structCode << "// Generated from: " << inputPath << "\n";
                    structCode << "struct alignas(16) " << structName << " {\n";

                    visitorCode << "    // Automatically generated reflection visitor for ImGui and Serialization\n";
                    visitorCode << "    template<typename F>\n";
                    visitorCode << "    void reflect(F&& visitor) {\n";

                    for (uint32_t m = 0; m < binding->block.member_count; m++) {
                        SpvReflectBlockVariable& member = binding->block.members[m];
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

                            // Nutze die Daten aus deiner shader_comment_parser.hpp
                            if (uiMetadata.find(member.name) != uiMetadata.end()) {
                                const auto& meta = uiMetadata[member.name];
                                if (meta.widgetType == "Slider") {
                                    minVal = meta.minBound;
                                    maxVal = meta.maxBound;
                                }
                            }

                            // Übergere die gemessenen Grenzen an die generierte reflect()-Methode
                            if (minVal != maxVal) {
                                visitorCode << "        visitor(\"" << member.name << "\", " << member.name << ", " << enumType << ", " << minVal << "f, " << maxVal << "f);\n";
                            }
                            else {
                                visitorCode << "        visitor(\"" << member.name << "\", " << member.name << ", " << enumType << ");\n";
                            }
                        }
                    }

                    visitorCode << "    }\n";

                    structCode << "\n" << visitorCode.str();
                    structCode << "}; // Total Size: " << currentSize << " Bytes\n";
                    structCode << "static_assert(sizeof(" << structName << ") % 16 == 0, \"Alignment Error!\");\n\n";

                    generatedStructs[structName] = { currentSize, structCode.str() };
                }
            }
        }
        spvReflectDestroyShaderModule(&module);
    }
    for (const auto& pair : generatedStructs) {
        outFile << pair.second.code;
    }

    outFile << "} // namespace Andromeda::Generated\n";
    outFile.close();
    return 0;
}