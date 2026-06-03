#pragma once

#include <regex>
#include <unordered_map>
#include <fstream>
#include <string>

/**
 * @struct PropertyUiMetadata
 * @brief Holds the extracted UI rendering metadata for a specific shader variable.
 * * This structure stores the configuration hints parsed from GLSL annotations,
 * which dictate how the property should be visualized inside the editor's inspector.
 */
struct PropertyUiMetadata {
    std::string widgetType = "Drag"; /**< The target ImGui widget type (e.g., "Drag", "Slider", "Color"). Defaults to "Drag". */
    float minBound = 0.0f;           /**< The minimum value constraint for slider/drag widgets. */
    float maxBound = 0.0f;           /**< The maximum value constraint for slider/drag widgets. */
};

/**
 * @brief Parses a GLSL source file to extract custom editor annotations declared directly above variables.
 * * This function reads the raw GLSL text line-by-line, looking for comment patterns like
 * `// @Editor: Slider(0.0, 1.0)` or `// @Editor: Color`. It maps the extracted metadata
 * to the variable name declared on the subsequent line.
 * * @param glslFilename Path to the original GLSL shader source file (.frag, .vert, etc.).
 * @return std::unordered_map<std::string, PropertyUiMetadata> A map linking variable names to their UI metadata.
 */
std::unordered_map<std::string, PropertyUiMetadata> ParseGlslAnnotations(const std::string& glslFilename) {
    std::unordered_map<std::string, PropertyUiMetadata> metadataMap;
    std::ifstream file(glslFilename);
    if (!file.is_open()) return metadataMap; // Return empty defaults if the source file is missing

    std::string line;
    PropertyUiMetadata pendingMetadata;
    bool hasPendingMetadata = false;

    // Regex to match patterns like "@Editor: Slider(0.0, 1.0)" or "@Editor: Color"
    std::regex editorRegex(R"(//\s*@Editor:\s*(\w+)(?:\(([0-9.-]+)\s*,\s*([0-9.-]+)\))?)");

    while (std::getline(file, line)) {
        std::smatch match;
        if (std::regex_search(line, match, editorRegex)) {
            pendingMetadata.widgetType = match[1].str();
            if (match[2].matched && match[3].matched) {
                pendingMetadata.minBound = std::stof(match[2].str());
                pendingMetadata.maxBound = std::stof(match[3].str());
            }
            else {
                pendingMetadata.minBound = 0.0f;
                pendingMetadata.maxBound = 0.0f;
            }
            hasPendingMetadata = true;
            continue;
        }

        // If metadata is queued, scan the next non-annotation line for the variable identifier (e.g., "float roughness;")
        if (hasPendingMetadata) {
            std::regex nameRegex(R"(\b\w+\b\s+(\w+)\s*[;\[])");
            std::smatch nameMatch;
            if (std::regex_search(line, nameMatch, nameRegex)) {
                std::string variableName = nameMatch[1].str();
                metadataMap[variableName] = pendingMetadata;
            }
            hasPendingMetadata = false; // Reset look-ahead state for the next pass
        }
    }
    return metadataMap;
}