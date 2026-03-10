#include "a_filesystem.hpp"

#include <filesystem>
namespace Andromeda {
    /// <summary>
    /// This function sets the device type based on the path.
    /// Sets the device type to default if not found.
    /// </summary>
    /// <param name="path">path which used to determine the device type based on the directory</param>
    /// <returns></returns>
    deviceType Filesystem::findDeviceTypeByPath(const std::string& path) {
        std::string lowerString = path;
        stringToLower(lowerString);
        for (auto& [key, value] : m_DirectoryNames) {
            if (lowerString.find(key) != std::string::npos) {
                return value;
            }
        }
        std::printf("[WARNING]: Device type not found");
        return deviceType::DEFAULT;
    }

    std::vector<std::string> Filesystem::getAllFilesInDirectoryRecursive(const std::string& directory, std::span<const std::string> filter)
    {
        std::vector<std::string> filePaths;
        std::error_code ec;

        std::filesystem::recursive_directory_iterator it(directory, std::filesystem::directory_options::skip_permission_denied, ec);

        for (const auto& entry : it) {
            if (ec) break;
            if (!entry.is_regular_file(ec)) continue;

            const std::string ext = entry.path().extension().generic_string();
            const bool allowed =
                filter.empty() ||
                std::find(filter.begin(), filter.end(), ext) != filter.end();

            if (allowed) {
                filePaths.emplace_back(entry.path().generic_string());
            }
        }

        return filePaths;
    }

    std::vector<std::string> Filesystem::getAllFilesInDirectory(const std::string& directory, std::span<std::string> filter)
    {
        std::vector<std::string> filePaths;
        std::error_code ec;
        for (const auto& entry : std::filesystem::directory_iterator(directory, ec)) {
            if (ec) break;
            if (!entry.is_regular_file()) continue;

            std::string ext = entry.path().extension().generic_string();
            bool allowed = filter.empty() ||
                std::find(filter.begin(), filter.end(), ext) != filter.end();

            if (allowed) {
                filePaths.emplace_back(entry.path().generic_string());
            }
        }
        return filePaths;
    }

    std::vector<std::string> Filesystem::getAllFilesInDirectory(const std::string& directory)
    {
        std::vector<std::string> filePaths;
        std::error_code ec;
        for (const auto& entry : std::filesystem::directory_iterator(directory, ec)) {
            if (ec) break;
            if (!entry.is_regular_file()) continue;
            filePaths.emplace_back(entry.path().generic_string());
        }
        return filePaths;
    }

    std::string Filesystem::getFileName(const std::string& path) const
    {
        size_t namePos = path.find_last_of("/");
        std::string objectName = (namePos == std::string::npos) ? path : path.substr(namePos + 1);

        size_t dotPos = objectName.find_last_of('.');
        std::string fileName = (dotPos == std::string::npos) ? objectName : objectName.substr(0, dotPos);
        return fileName;
    }
}