#pragma once
#include <vector>
#include <unordered_map>
#include <string>
#include <memory>
#include <assimp/scene.h>
#include "shader.hpp"
#include "a_components.hpp"
#include "a_texture.hpp"
#include "a_device.hpp"
#include "a_opengl_handles.hpp"
#include "a_primitives.hpp"
#include "a_ISubsystem.hpp"

namespace Andromeda {

    /**
     * @brief Central hub for managing engine assets like Shaders, Meshes, and Textures.
     * * Implements ISubsystem for lifecycle management and IDeviceProvider to supply
     * hardware-related data and icons to the GUI/Editor.
     */
    class ResourceManager : public ISubsystem, public IDeviceProvider {
    public:

        // --- Shader Storage ---
        std::vector<std::unique_ptr<MaterialShader>> m_MaterialShaders;
        std::vector<std::unique_ptr<ProceduralShader>> m_ProceduralShaders;
        std::vector<std::unique_ptr<PostProcessShader>> m_PostProcessShaders;

        /// Maps device types to their respective UI icons (OpenGL texture handles)
        std::unordered_map<deviceType, GLtexture> m_DeviceIcons;

        // --- Subsystem Interface ---
        static constexpr std::string_view GetStaticName() { return "ResourceManager"; }
        const char* getSubsystemName() const override { return GetStaticName().data(); }

        void updateEvent(SDL_Event* event) override;
        void start() override;
        void update() override;

        /**
         * @brief Loads 3D models and initializes their GPU handles.
         */
        void loadModels();

        /**
         * @brief Loads UI icons from disk and creates OpenGL textures.
         */
        void loadIcons();

        /**
         * @brief Resolves a device type based on its icon's string name.
         */
        deviceType findDeviceIcon(std::string iconName);

        // --- Resource Accessors (Mesh & Shaders) ---
        u32 getMeshVaoByID(uint32_t meshID) const;
        u32 getMeshIndexSizeByID(uint32_t meshID) const;
        const Mesh& getMeshByID(uint32_t meshID) const;

        MaterialShader* getMaterialShaderByID(MaterialShaderType t) const;
        ProceduralShader* getProceduralShaderByID(ProceduralShaderType t) const;
        PostProcessShader* getPostprocessShaderByID(PostProcessShaderType t) const;

        // Wrapper function for getting the amount of recorded devices
        u32 getDeviceRecordsSize() const;
        const std::unordered_map<uint32_t, Device>& getDeviceRecords() const;

        /**
         * @brief Utility to create an OpenGL texture from a file path.
         */
        GLtexture CreateOpenGLTexture(const char* path);

        u32 getDeviceCount() const override;
        const Device& getDeviceData(u32 index) const override;
        virtual u32 getDeviceIconID(deviceType type) const;

    private:
        /// Internal storage for device metadata
        std::unordered_map<uint32_t, Device> m_DeviceRecords;

        /// Links Mesh IDs to their OpenGL VAO/VBO handles
        std::unordered_map<i32, MeshGPUHandle> m_GPUMeshes;

        /// Maintainer of the display order for the Device Browser UI
        std::vector<u32> m_DeviceIndexList;

        /// Helper map to find internal IDs by their mesh filenames/names
        std::unordered_map<std::string, uint32_t> m_MeshIDbyName;
        unsigned int m_NextMeshID = 0;

        // --- Template Helpers for Type-Safe Shader Registration ---

        template<typename T> requires std::derived_from<T, MaterialShader>
        void addMaterialShader(const char* vertexShader, const char* fragmentShader) {
            m_MaterialShaders.emplace_back(std::make_unique<T>(vertexShader, fragmentShader));
            m_MaterialShaders.back()->compileShader();
        }

        template<typename T> requires std::derived_from<T, ProceduralShader>
        void addProceduralShader(const char* vertexShader, const char* fragmentShader) {
            m_ProceduralShaders.emplace_back(std::make_unique<T>(vertexShader, fragmentShader));
            m_ProceduralShaders.back()->compileShader();
        }

        template<typename T> requires std::derived_from<T, PostProcessShader>
        void addPostProcessShader(const char* vertexShader, const char* fragmentShader) {
            m_PostProcessShaders.emplace_back(std::make_unique<T>(vertexShader, fragmentShader));
            m_PostProcessShaders.back()->compileShader();
        }

        // --- Internal Model Loading (Assimp Integration) ---

        void setupMeshes();
        void processNode(const uint32_t meshId, const aiScene* scene, aiNode* node);
        void loadScene(const std::string& path);
    };
}