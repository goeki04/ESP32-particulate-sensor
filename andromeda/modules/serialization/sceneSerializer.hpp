#pragma once

/**
 * @file sceneSerializer.hpp
 * @brief Saving and loading of the ECS scene state to and from JSON files.
 */

#include <string>
#include "a_registry.hpp"
#include "resource_manager.h"
namespace Andromeda {
	class ResourceManager;

	/**
	 * @class SceneSerializer
	 * @brief Persists and restores a @c ComponentRegistry (the whole scene) as JSON.
	 *
	 * @details Walks the registry's component pools to write/read each component type. Stateless;
	 *          all functionality is exposed through static methods.
	 *
	 * @warning Component pools are keyed by @c typeid(T).name(), which is compiler-specific.
	 *          See @c save() / @c load() for the cross-platform implications.
	 */
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
		static bool save(const std::string& filepath, ECS::ComponentRegistry& registry, ResourceManager& resources);
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
		static bool load(const std::string& filepath, ECS::ComponentRegistry& registry, ResourceManager& resources);
	};
}