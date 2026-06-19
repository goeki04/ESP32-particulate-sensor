#include "resource_manager.h"
#include "a_filesystem.hpp"
#include "a_geometry.hpp"
#include <string>
#define STB_IMAGE_IMPLEMENTATION

#include "stb_image.h"
#include <assimp/Importer.hpp>
#include "OpenGL/a_opengl_upload.hpp"
#include <assimp/postprocess.h>
#include "OpenGL/a_GLcubemap.hpp"
namespace Andromeda {
    void ResourceManager::start()
    {
        loadModels();
        loadDeviceIcons();
        loadEditorIcons();
        setupMeshes();
        loadAllCubeMaps();
        for (const auto& [name, data] : m_CubemapData) {
            std::printf("Cubemap Name: %s\n", name.c_str());
            std::printf("  - ID: %u\n", data.textureID);
            std::printf("  - Resolution: %dx%d\n", data.width, data.height);

            for (const auto& path : data.texturePath) {
                std::printf("    Path: %s\n", path.c_str());
            }
        }
    }

    void ResourceManager::loadModels()
    {
        std::array<std::string, 2> filter;
        filter[0] = ".fbx";
        filter[1] = ".obj";
        const std::vector<std::string> paths = Filesystem::getAllFilesInDirectoryRecursive(ASSET_PATH "models", filter);

        for (auto& v : paths) {
            loadScene(v);
        }
    }

    /**
 * @brief Scans and loads all cubemaps and environment maps from designated directories.
 * * This function handles two distinct loading paths:
 * 1. LDR Cubemaps: Scans the "/LDR" subdirectory for folders containing sets of 6 textures.
 * 2. HDR Panoramas: Scans the "/HDR" subdirectory for single-file .hdr or .hdri images.
 * *
 */
void ResourceManager::loadAllCubeMaps()
{
    std::vector<Filesystem::Directory> cubeMapLDR = Filesystem::getAllDirectories(CUBEMAP_PATH "/LDR");
    const std::vector<std::string> filter = {".hdr", ".hdri"};
    std::vector<std::string> cubeMapHDR = Filesystem::getAllFilesInDirectoryRecursive(CUBEMAP_PATH "/HDR", filter);

    if (!cubeMapLDR.empty())
    {
        for (auto& directory : cubeMapLDR)
        {
            loadAndStoreCubemap(directory.name, directory.files);
        }
    }

    if (!cubeMapHDR.empty())
    {
        for (std::string file : cubeMapHDR)
        {
            loadAndStoreCubemap(file);
        }
    }
}

/**
 * @brief Decodes image data from disk into CPU memory.
 * * Iterates through all registered paths in the CubemapData and uses stb_image
 * to load pixel data. It automatically switches between floating-point loading
 * for HDR and byte loading for standard LDR images.
 * * @param data Reference to the CubemapData structure to be populated with dimensions and pixels.
 * @note Pixel memory is allocated via stb_image and must be freed using CubemapData::freePixelData().
 */
void ResourceManager::loadCubemapTexture(CubemapData& data){
        for (u32 i = 0; i < data.texturePath.size(); i++) {
            const char* path = data.texturePath[i].c_str();
            if (data.isHDR)
            {
                stbi_set_flip_vertically_on_load(true);
                float* pixels = stbi_loadf(path, &data.width, &data.height, &data.channels, 0);
                data.pixelData[i] = pixels;
            }
            else
            {
                stbi_set_flip_vertically_on_load(false);
                unsigned char* pixels = stbi_load(path, &data.width, &data.height, &data.channels, 0);
                data.pixelData[i] = static_cast<void*>(pixels);
            }

            if (!data.pixelData[i])
            {
                std::printf("[ERROR]: Failed to load cubemap face: %s\n", path);
            }
        }
}

/**
 * @brief Maps a file path to its corresponding cube face index based on naming conventions.
 * * Standard OpenGL Cubemap indexing:
 * - 0: Positive X (Right)
 * - 1: Negative X (Left)
 * - 2: Positive Y (Top)
 * - 3: Negative Y (Bottom)
 * - 4: Positive Z (Back)
 * - 5: Negative Z (Front)
 * *
 * * @param data The CubemapData structure where the path will be assigned.
 * @param path The file path to analyze.
 */
void ResourceManager::MapCubemapFacesLDR(CubemapData& data, const std::string& path)
{
    std::string fileName = Filesystem::getFileName(path);

    std::string lowerName = fileName;
    std::ranges::transform(lowerName, lowerName.begin(),
                           [](const unsigned char c){ return std::tolower(c); });

    if (lowerName.find("right") != std::string::npos)       data.texturePath[0] = path;
    else if (lowerName.find("left") != std::string::npos)   data.texturePath[1] = path;
    else if (lowerName.find("top") != std::string::npos)    data.texturePath[2] = path;
    else if (lowerName.find("bottom") != std::string::npos) data.texturePath[3] = path;
    else if (lowerName.find("back") != std::string::npos)   data.texturePath[4] = path;
    else if (lowerName.find("front") != std::string::npos)  data.texturePath[5] = path;
    else {
        std::printf("[WARNING]: Could not map cubemap face for file: %s\n", path.c_str());
    }
}
    /**
 * @brief Loads a cubemap from a set of 6 individual image files (LDR).
 * * This function is used for traditional sky boxes where each face of the cube
 * (+X, -X, +Y, -Y, +Z, -Z) is stored in a separate file. The files are
 * automatically mapped to their respective cube faces based on their filenames.
 * * @param name The unique identifier (key) to store the cubemap in the ResourceManager.
 * @param paths A vector containing exactly 6 file paths to the texture images.
 * * @note If the vector does not contain exactly 6 paths, the loading process
 * will be aborted and a warning will be logged to the console.
 * @warning Ensure that CubemapGL::CubemapTextureUploadGL(data) is called before
 * moving the data into the map to finalize the GPU upload.
 */
void ResourceManager::loadAndStoreCubemap(const std::string& name, const std::vector<std::string>& paths) {
    constexpr i32 cubemapSizeLDR = 6;
    if (paths.size() == cubemapSizeLDR)
    {
        CubemapData data;
        data.texturePath.resize(6);
        for (u32 i = 0; i < cubemapSizeLDR; i++)
        {
            MapCubemapFacesLDR(data, paths[i]);
        }

        loadCubemapTexture(data);
        CubemapGL::CubemapTextureUploadGL(data);
        m_CubemapData[name] = std::move(data);
    }
    else
    {
        std::printf("[WARNING]: Cubemap directory '%s' contains %zu files! Skipping...\n",
                name.c_str(), paths.size());
    }
}

/**
 * @brief Loads an environment map from a single HDR panorama file.
 * * This function automatically detects if the file is in a high dynamic range format.
 * The image is treated as an equirectangular panorama, commonly used for
 * Image Based Lighting (IBL) or high-quality sky boxes.
 * * @param file The full system path to the HDR/HDRI file.
 * * @note This sets the isHDR flag within CubemapData, which switches the
 * OpenGL upload process to use GL_RGB16F and GL_FLOAT for high precision.
 * @warning Ensure that CubemapGL::CubemapTextureUploadGL(data) is called before
 * moving the data into the map to finalize the GPU upload.
 */
void ResourceManager::loadAndStoreCubemap(const std::string& file) {
        if (stbi_is_hdr(file.c_str()))
        {
            CubemapData data;
            data.isHDR = true;
            data.texturePath.push_back(file);

            loadCubemapTexture(data);
            CubemapGL::CubemapTextureUploadGL(data);
            std::string name = Filesystem::getFileName(file);
            m_CubemapData[name] = std::move(data);
        }
}

    void ResourceManager::loadDeviceIcons()
    {
        std::array<std::string, 2> filter;
        filter[0] = ".png";
        filter[1] = ".jpg";
        const std::vector<std::string> paths = Filesystem::getAllFilesInDirectory(ASSET_PATH "icons/device/", filter);

        for (auto& v : paths) {
            std::string fileName = Filesystem::getFileName(v);
            deviceType type = findDeviceIcon(fileName);
            m_DeviceIcons[type] = CreateOpenGLTexture(v.c_str());
        }
    }

    u32 ResourceManager::getEditorIconID(const std::string& name)
    {
        auto it = m_EditorIcons.find(name);
        if (it != m_EditorIcons.end()) {
            return it->second.id;
        }
        throw std::runtime_error("Icon not found: " + name + "!");
    }

    u32 ResourceManager::registerCustomMesh(Mesh&& mesh, const std::string& name)
    {
        const uint32_t id = m_NextMeshID++;

        m_Meshes[id] = std::move(mesh);

        MeshGPUHandle& gpuHandle = m_GPUMeshes[id];
        createMesh(gpuHandle, m_Meshes[id]);

        m_ModelRecords[id] = ModelRecord{ id, name, deviceType::DEFAULT };
        m_ModelIndexList.push_back(id);
        m_MeshIDbyName.emplace(name, id);

        return id;
    }

    void ResourceManager::loadEditorIcons()
    {
        std::array<std::string,2> filter;
        filter[0] = ".png";
        filter[1] = ".jpg";
        const std::vector<std::string> paths = Filesystem::getAllFilesInDirectory(ASSET_PATH "icons/editor", filter);
        for (auto& v : paths)
        {
            std::string fileName = Filesystem::getFileName(v);
            m_EditorIcons[fileName] = CreateOpenGLTexture(v.c_str());
        }
    }

    deviceType ResourceManager::findDeviceIcon(const std::string &iconName) {
        for (auto& v : Filesystem::m_DirectoryNames) {
            if (v.first.find(iconName) != std::string::npos) {
                return v.second;
            }
        }
        return deviceType::DEFAULT;
    };

    u32 ResourceManager::getMeshVaoByID(const u32 meshID) const
    {
		auto it = m_GPUMeshes.find(meshID);
		if (it != m_GPUMeshes.end())
		{
			return it->second.vao;
		}
        std::printf("[WARNING]: Mesh ID %u not found!\n", meshID);
        return 0;
    }

    u32 ResourceManager::getMeshIndexSizeByID(const u32 meshID) const {
		auto it = m_Meshes.find(meshID);
        if (it != m_Meshes.end())
        {
			return it->second.indexBuffer.size();
        }
        std::printf("[WARNING]: Mesh ID %u not found!\n", meshID);
        return 0;
    }

    const Mesh& ResourceManager::getMeshByID(const u32 meshID) const
    {
        return m_Meshes.at(meshID);
    }

    u32 ResourceManager::getModelRecordsSize() const
    {
        return m_ModelRecords.size();
    }

    const std::unordered_map<uint32_t, ModelRecord>& ResourceManager::getModelRecords() const
    {
        return m_ModelRecords;
    }

    u32 ResourceManager::getModelCount() const {
        return static_cast<u32>(m_ModelRecords.size());
    }
    const ModelRecord& ResourceManager::getModelData(u32 index) const {
        u32 modelID = m_ModelIndexList.at(index);
        return m_ModelRecords.at(modelID);
    }

    u32 ResourceManager::getDeviceIconID(const deviceType type) const {
        return m_DeviceIcons.at(type).id;
    }

    GLtexture ResourceManager::CreateOpenGLTexture(const char* path)
    {
        GLtexture texture;
        i32 w, h, channels;
        unsigned char* pixels = stbi_load(path, &w, &h, &channels, 4);
        if (!pixels) {
            const std::string log = stbi_failure_reason();
            throw std::runtime_error("Failed to load image: " + log);
        }

        texture.w = w; texture.h = h;

        glGenTextures(1, &texture.id);
        glBindTexture(GL_TEXTURE_2D, texture.id);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

        stbi_image_free(pixels);
        return texture;
    }


    void ResourceManager::setupMeshes()
    {
        for (auto& [id, mesh] : m_Meshes)
        {
            MeshGPUHandle& gpuHandle = m_GPUMeshes[id];
            createMesh(gpuHandle, mesh);
        }
    }
    void ResourceManager::processNode(const uint32_t meshId, const aiScene* scene, aiNode* node)
    {
        auto& newMesh = m_Meshes.at(meshId);
        for (unsigned int i = 0; i < node->mNumMeshes; i++) {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            aiColor3D color(1.0f, 1.0f, 1.0f);
            aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

            std::vector<unsigned int> indexBuffer;
            std::vector<Vertex> vertices;
            vertices.reserve(mesh->mNumVertices);

            bool hasNormals = mesh->HasNormals();
            if (!hasNormals) {
                std::printf("Vertex doesnt have normals");
            }
            material->Get(AI_MATKEY_COLOR_DIFFUSE, color);
            for (unsigned int j = 0; j < mesh->mNumVertices; j++) {
                Vertex vertex{};
                aiVector3D worldPos = mesh->mVertices[j];
                vertex.pos = glm::vec3(worldPos.x, worldPos.y, worldPos.z);

                if (hasNormals) {
                    vertex.normal = glm::vec3(mesh->mNormals[j].x, mesh->mNormals[j].y, mesh->mNormals[j].z);
                }

                if (mesh->HasTextureCoords(0)) {
                    vertex.uv = glm::vec2(mesh->mTextureCoords[0][j].x, mesh->mTextureCoords[0][j].y);
                }
                vertex.color = glm::vec3(color.r, color.g, color.b);
                vertices.push_back(vertex);
            }

            auto& vb = newMesh.vertexbuffer;
            auto& ib = newMesh.indexBuffer;

            const uint32_t baseVertex = static_cast<uint32_t>(vb.size());

            vb.insert(vb.end(), vertices.begin(), vertices.end());

            for (unsigned int k = 0; k < mesh->mNumFaces; k++) {
                const aiFace& f = mesh->mFaces[k];
                for (unsigned int t = 0; t < f.mNumIndices; t++) {
                    ib.push_back(baseVertex + f.mIndices[t]);
                }
            }
        }
        for (int i = 0; i < node->mNumChildren; i++) {
            processNode(meshId, scene, node->mChildren[i]);
        }
    }

    void ResourceManager::loadScene(const std::string& path) {
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(
            path.c_str(),
            aiProcess_Triangulate |
            aiProcess_ConvertToLeftHanded |
            aiProcess_FlipUVs |
            aiProcess_GenNormals
        );

        if (!scene) {
            throw std::runtime_error(importer.GetErrorString());
        }

        std::string meshName = Filesystem::getFileName(path);
        if (m_MeshIDbyName.contains(meshName)) {
            std::printf("Mesh already exists!\n");
            return;
        }

        const uint32_t id = m_NextMeshID++;
        const deviceType dt = Filesystem::findDeviceTypeByPath(path);

        m_Meshes[id] = Andromeda::Mesh{};

        m_ModelRecords[id] = ModelRecord{id,meshName,dt};
        m_ModelIndexList.push_back(id);

        m_MeshIDbyName.emplace(meshName, id);

        aiNode* rootNode = scene->mRootNode;
        processNode(id, scene, rootNode);
    }


    ShaderProgramHandle ResourceManager::loadShaderRHI(IGraphicsContext* ctx, const std::string& name, const std::string& vertPath, const std::string& fragPath) {
        auto it = m_RhiShaders.find(name);
        if (it != m_RhiShaders.end()) {
            return it->second;
        }

        ShaderProgramHandle handle = ctx->createShaderProgram(vertPath, fragPath);
        m_RhiShaders[name] = handle;
        return handle;
    }

    ShaderProgramHandle ResourceManager::getShaderRHI(const std::string& name) const {
        auto it = m_RhiShaders.find(name);
        if (it != m_RhiShaders.end()) [[likely]] {
            return it->second;
        }
        return ShaderProgramHandle{ 0 };
    }

    std::shared_ptr<Material> ResourceManager::createMaterial(const std::string& materialName, ShaderProgramHandle shaderHandle, IGraphicsContext* ctx) {
        auto it = m_Materials.find(materialName);
        if (it != m_Materials.end()) {
            return it->second;
        }

        auto newMaterial = std::make_shared<Material>(shaderHandle, ctx);
        m_Materials[materialName] = newMaterial;

        return newMaterial;
    }

    std::shared_ptr<Material> ResourceManager::getMaterial(const std::string& materialName) const {
        auto it = m_Materials.find(materialName);
        if (it != m_Materials.end()) [[likely]] {
            return it->second;
        }
        return nullptr;
    }

    void ResourceManager::destroyRHIResources(IGraphicsContext* ctx) {
        for (auto& [name, handle] : m_RhiShaders) {
            ctx->destroyShaderProgram(handle);
        }
        m_RhiShaders.clear();
        m_Materials.clear();
    }
}