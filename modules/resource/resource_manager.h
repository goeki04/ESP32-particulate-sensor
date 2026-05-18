#pragma once
#include <vector>
#include <unordered_map>
#include <string>
#include <memory>
#include <assimp/scene.h>
#include "shader.hpp"
#include "a_components.hpp"
#include "a_texture.hpp"
#include "a_geometry.hpp"
#include "a_model_record.hpp"
#include "a_opengl_handles.hpp"
#include "a_primitives.hpp"
#include "a_ISubsystem.hpp"
#include "a_cubemapData.hpp"
namespace Andromeda {

    /**
     * @brief Central hub for managing engine assets like Shaders, Meshes, and Textures.
     * * Implements ISubsystem for lifecycle management and IDeviceProvider to supply
     * hardware-related data and icons to the GUI/Editor.
     */
    class ResourceManager : public ISubsystem, public IModelProvider {
    public:

        // --- Shader Storage ---
        std::vector<std::unique_ptr<MaterialShader>> m_MaterialShaders;
        std::vector<std::unique_ptr<ProceduralShader>> m_ProceduralShaders;
        std::vector<std::unique_ptr<PostProcessShader>> m_PostProcessShaders;
        std::unordered_map<u32, Andromeda::Mesh> m_Meshes;
        /// Maps device types to their respective UI icons (OpenGL texture handles)
        std::unordered_map<deviceType, GLtexture> m_DeviceIcons;
        std::unordered_map<std::string, GLtexture> m_EditorIcons;
        std::unordered_map<std::string, CubemapData> m_CubemapData;
        // --- Subsystem Interface ---
        static constexpr std::string_view GetStaticName() { return "ResourceManager"; }
        [[nodiscard]] const char* getSubsystemName() const override { return GetStaticName().data(); }

        void start() override;

        /**
         * @brief Loads 3D models and initializes their GPU handles.
         */
        void loadModels();
        void loadAllCubeMaps();

        /**
         * @brief Loads UI icons from disk and creates OpenGL textures.
         */
        void loadDeviceIcons();
        void loadEditorIcons();
        u32 getEditorIconID(const std::string& name);
        /**
         * @brief Resolves a device type based on its icon's string name.
         */
        static deviceType findDeviceIcon(const std::string &iconName);

        // --- Resource Accessors (Mesh & Shaders) ---
        [[nodiscard]] u32 getMeshVaoByID(u32 meshID) const;
        [[nodiscard]] u32 getMeshIndexSizeByID(u32 meshID) const;
        [[nodiscard]] const Mesh& getMeshByID(u32 meshID) const;

        [[nodiscard]] MaterialShader* getMaterialShaderByID(MaterialShaderType t) const;
        [[nodiscard]] ProceduralShader* getProceduralShaderByID(ProceduralShaderType t) const;
        [[nodiscard]] PostProcessShader* getPostprocessShaderByID(PostProcessShaderType t) const;

        // Wrapper function for getting the amount of recorded devices
        [[nodiscard]] u32 getModelRecordsSize() const;
        [[nodiscard]] const std::unordered_map<uint32_t, ModelRecord>& getModelRecords() const;

        /**
         * @brief Utility to create an OpenGL texture from a file path.
         */
        static GLtexture CreateOpenGLTexture(const char* path);

        void loadAndStoreCubemap(const std::string& name, const std::vector<std::string>& paths);
        void loadAndStoreCubemap(const std::string& file);
        [[nodiscard]] u32 getModelCount() const override;
        [[nodiscard]] const ModelRecord& getModelData(u32 index) const override;

        [[nodiscard]] u32 getDeviceIconID(deviceType type) const override;

    private:
        static void loadCubemapTexture(CubemapData& data);
        static void MapCubemapFacesLDR(CubemapData& data, const std::string& path);
        /// Internal storage for device metadata
        std::unordered_map<u32, ModelRecord> m_ModelRecords;

        /// Links Mesh IDs to their OpenGL VAO/VBO handles
        std::unordered_map<i32, MeshGPUHandle> m_GPUMeshes;

        /// Maintainer of the display order for the Device Browser UI
        std::vector<u32> m_ModelIndexList;

        /// Helper map to find internal IDs by their mesh filenames/names
        std::unordered_map<std::string, u32> m_MeshIDbyName;
        u32 m_NextMeshID = 0;


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
        void processNode(uint32_t meshId, const aiScene* scene, aiNode* node);
        void loadScene(const std::string& path);
    };
}