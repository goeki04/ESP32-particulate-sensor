#pragma once

/**
 * @file a_filesystem.hpp
 * @brief Directory/file enumeration helpers and device-type mapping based on folder naming conventions.
 */

#include <array>
#include <string_view>
#include <string>
#include <algorithm>
#include <span>
#include <vector>
#include "a_model_record.hpp"
namespace Andromeda {
	/**
	 * @namespace Andromeda::Filesystem
	 * @brief Free functions and helpers for traversing the asset directory tree.
	 */
	namespace Filesystem{

		/**
		 * @struct Directory
		 * @brief A directory's name together with the list of files it contains.
		 */
		struct Directory
		{
			std::string name;                ///< Name of the directory (typically the leaf folder name).
			std::vector<std::string> files;  ///< Paths/names of the files contained within.
		};

		/**
		 * @brief Maps reserved asset subfolder names to the device type they contain.
		 * @details Used by @c findDeviceTypeByPath() to classify models by the folder they live in
		 *          (e.g. a model under "dsensor" is treated as a @c deviceType::SENSOR).
		 */
		static constexpr std::array<std::pair<std::string_view, deviceType>, 4> m_DirectoryNames{ {
			{"dsensor",     deviceType::SENSOR},
			{"dcontroller", deviceType::CONTROLLER},
			{"dcable",      deviceType::CABLE},
			{"dbreadboard", deviceType::BREADBOARD},
		} };

		/**
		 * @brief Converts a string to lowercase in place (ASCII).
		 * @param data The string to transform; modified directly.
		 */
		inline void stringToLower(std::string& data) {
				std::ranges::transform(data, data.begin(),
				                       [](const unsigned char c) {return std::tolower(c);
				                       });
		}

		/**
		 * @brief Determines the device type encoded in a path via the @c m_DirectoryNames convention.
		 * @param path The file or directory path to inspect.
		 * @return The matched @c deviceType, or @c deviceType::DEFAULT if no reserved folder name is present.
		 */
		deviceType findDeviceTypeByPath(const std::string& path);

		/**
		 * @brief Recursively collects all files under a directory whose extension matches the filter.
		 * @param directory Root directory to search.
		 * @param filter Allowed file extensions; pass an empty span to accept all files.
		 * @return Paths of all matching files found at any depth.
		 */
		std::vector<std::string> getAllFilesInDirectoryRecursive(const std::string& directory, std::span<const std::string> filter);

		/**
		 * @brief Collects files directly inside a directory (non-recursive) matching the filter.
		 * @param directory Directory to search.
		 * @param filter Allowed file extensions; pass an empty span to accept all files.
		 * @return Paths of the matching files in the top level of @p directory.
		 */
		std::vector<std::string> getAllFilesInDirectory(const std::string& directory, std::span<std::string> filter);

		/**
		 * @brief Collects all files directly inside a directory (non-recursive, unfiltered).
		 * @param directory Directory to search.
		 * @return Paths of all files in the top level of @p directory.
		 */
		std::vector<std::string> getAllFilesInDirectory(const std::string& directory);

		/**
		 * @brief Extracts the file name (with extension) from a full path.
		 * @param path The full file path.
		 * @return The final path component.
		 */
		std::string getFileName(const std::string& path);

		/**
		 * @brief Builds a @c Directory record (name + contained files) for a single directory.
		 * @param path Path of the directory to describe.
		 * @return The populated @c Directory.
		 */
		Directory getDirectory(const std::string& path);

		/**
		 * @brief Enumerates all immediate subdirectories of a path, each as a @c Directory record.
		 * @param path The parent directory to scan.
		 * @return One @c Directory entry per subfolder.
		 */
		std::vector<Directory> getAllDirectories(const std::string& path);
	};
}