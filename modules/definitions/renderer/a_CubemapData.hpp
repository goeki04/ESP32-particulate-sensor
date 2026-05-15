#pragma once
#include "a_primitives.hpp"
#include <vector>
#include <stb_image.h>
namespace Andromeda{
    struct CubemapData{
        std::array<void*, 6> pixelData = {
            nullptr
        };
        std::vector<std::string> texturePath;
        u32 textureID = 0;
        i32 width;
        i32 height;
        i32 channels;

        bool isHDR = false;

        CubemapData() = default;
        CubemapData(const CubemapData&) = delete;
        CubemapData& operator=(const CubemapData&) = delete;

        CubemapData(CubemapData&& other) noexcept :
            pixelData(other.pixelData),
            texturePath(other.texturePath),
            textureID(other.textureID),
            width(other.width), height(other.height), channels(other.channels) {
            other.pixelData.fill(nullptr);
        }

        CubemapData& operator=(CubemapData&& other) noexcept {
            if (this != &other) {
                freePixelData();
                pixelData = other.pixelData;
                texturePath = other.texturePath;
                width = other.width;
                textureID = other.textureID;
                height = other.height;
                channels = other.channels;
                other.pixelData.fill(nullptr);
            }
            return *this;
        }

        void freePixelData() {
            for (auto*& ptr : pixelData) {
                if (ptr) stbi_image_free(ptr);
            }
        }

        ~CubemapData() {
            freePixelData();
        }
    };
}