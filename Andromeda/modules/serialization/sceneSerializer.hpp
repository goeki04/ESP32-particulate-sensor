#pragma once
#include <string>
#include "a_registry.hpp"
namespace Andromeda {
	class SceneSerializer{
	public:
        /**
     * @brief Serializes the current ECS state into a JSON file.
     * * Iterates through all component pools in the registry and stores their
     * data using the type name as the primary key.
     * * @param filepath Path to the destination file.
     * @param registry Reference to the ECS registry to be saved.
     * @return true if saving was successful, false otherwise.
     * * @warning @b CROSS-PLATFORM-ISSUE: This function uses `typeid(T).name()` as a key.
     * Compilers like MSVC (Windows) and GCC (Linux) produce different strings for the same type.
     * Files saved on Windows might not be loadable on Linux.
     * * @note @b SOLUTION: Replace `typeid(T).name()` with a manual string identifier
     * (e.g., a `static constexpr const char* name` inside each component struct).
     */
		static bool save(const std::string& filepath, ECS::ComponentRegistry& registry);
        /**
     * @brief Deserializes ECS data from a JSON file into the registry.
     * * This function attempts to find known component pools within the JSON structure.
     * It reconstructs the Sparse-Set for each pool and updates the global entity counter.
     * * @param filepath Path to the source file.
     * @param registry Reference to the ECS registry to be populated.
     * @return true if loading was successful, false otherwise.
     * * @bug If the compiler-generated type name in the file does not match the current
     * platform's `typeid(T).name()`, the pool will be skipped.
     */
		static bool load(const std::string& filepath, ECS::ComponentRegistry& registry);
	};
}