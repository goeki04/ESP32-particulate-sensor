#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstring>
#include "spirv_reflect.h"

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
        if (type->traits.numeric.matrix.column_count == 4 && type->traits.numeric.matrix.row_count == 4) return "glm::mat4";
        if (type->traits.numeric.matrix.column_count == 3 && type->traits.numeric.matrix.row_count == 3) return "glm::mat3";
    }
    if (type->type_flags & SPV_REFLECT_TYPE_FLAG_VECTOR) {
        if (type->traits.numeric.vector.component_count == 4) return "glm::vec4";
        if (type->traits.numeric.vector.component_count == 3) return "glm::vec3";
        if (type->traits.numeric.vector.component_count == 2) return "glm::vec2";
    }
    if (type->type_flags & SPV_REFLECT_TYPE_FLAG_FLOAT) return "float";
    if (type->type_flags & SPV_REFLECT_TYPE_FLAG_INT) return "int";

    return "UNKNOWN_TYPE";
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
    outFile << "#include \"a_primitives.hpp\"\n\n";
    outFile << "namespace Andromeda::Generated {\n\n";

    for (int i = 2; i < argc; ++i) {
        std::string inputPath = argv[i];

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

        outFile << "// Generated from: " << inputPath << "\n";

        for (uint32_t b = 0; b < bindingCount; b++) {
            SpvReflectDescriptorBinding* binding = bindings[b];

            if (binding->descriptor_type == SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER) {
                std::string structName = "";
                if (binding->name && strlen(binding->name) > 0) structName = binding->name;
                else if (binding->type_description && binding->type_description->type_name) structName = binding->type_description->type_name;
                else structName = "UnknownBuffer_Binding" + std::to_string(binding->binding);

                outFile << "struct " << structName << " {\n";

                for (uint32_t m = 0; m < binding->block.member_count; m++) {
                    SpvReflectBlockVariable& member = binding->block.members[m];
                    std::string cppType = GetCppType(member.type_description);

                    outFile << "    // Offset: " << member.offset << "\n";
                    if (cppType.find("vec") != std::string::npos || cppType.find("mat") != std::string::npos) {
                        outFile << "    alignas(16) " << cppType << " " << member.name << ";\n";
                    }
                    else {
                        outFile << "    " << cppType << " " << member.name << ";\n";
                    }
                }
                outFile << "}; // Total Size: " << binding->block.size << " Bytes\n";
                outFile << "static_assert(sizeof(" << structName << ") % 16 == 0, \"Alignment Error!\");\n\n";
            }
        }
        spvReflectDestroyShaderModule(&module);
    }

    outFile << "} // namespace Andromeda::Generated\n";
    outFile.close();
    return 0;
}