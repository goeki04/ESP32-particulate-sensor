#pragma once
#include "a_primitives.hpp"
#include <array>
#include <stb_image.h>
namespace Andromeda{
    struct CubemapData{
        std::array<unsigned char*, 6> pixelData = {
            nullptr
        };
        std::array<const char*,6> faceTexturePath;
        i32 width;
        i32 height;
        i32 channels;

        CubemapData() = default;
        CubemapData(const CubemapData&) = delete;
        CubemapData& operator=(const CubemapData&) = delete;

        CubemapData(CubemapData&& other) noexcept :
            pixelData(other.pixelData),
            faceTexturePath(other.faceTexturePath),
            width(other.width), height(other.height), channels(other.channels) {
            other.pixelData.fill(nullptr);
        }

        CubemapData& operator=(CubemapData&& other) noexcept {
            if (this != &other) {
                freePixelData();
                pixelData = other.pixelData;
                faceTexturePath = other.faceTexturePath;
                width = other.width;
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