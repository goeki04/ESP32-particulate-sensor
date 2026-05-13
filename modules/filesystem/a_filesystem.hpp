#pragma once
#include <array>
#include <string_view>
#include <string>
#include <algorithm>
#include <span>
#include <vector>
#include "a_device.hpp"
namespace Andromeda {
	namespace Filesystem{

		struct Directory
		{
			std::string directoryName;
			std::vector<std::string> files;
		};

		static constexpr std::array<std::pair<std::string_view, deviceType>, 4> m_DirectoryNames{ {
			{"dsensor",     deviceType::SENSOR},
			{"dcontroller", deviceType::CONTROLLER},
			{"dcable",      deviceType::CABLE},
			{"dbreadboard", deviceType::BREADBOARD},
		} };
		
		inline void stringToLower(std::string& data) {
				std::ranges::transform(data, data.begin(),
				                       [](const unsigned char c) {return std::tolower(c);
				                       });
		}
		deviceType findDeviceTypeByPath(const std::string& path);
		std::vector<std::string> getAllFilesInDirectoryRecursive(const std::string& directory, std::span<const std::string> filter);
		std::vector<std::string> getAllFilesInDirectory(const std::string& directory, std::span<std::string> filter);
		std::vector<std::string> getAllFilesInDirectory(const std::string& directory);
		std::string getFileName(const std::string& path);
		Directory getDirectory(const std::string& path);
		std::vector<Directory> getAllDirectories(const std::string& path);
	};
}