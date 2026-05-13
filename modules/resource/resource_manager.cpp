#include "resource_manager.h"
#include "a_filesystem.hpp"
#include <string>
#define STB_IMAGE_IMPLEMENTATION

#include "stb_image.h"
#include <assimp/Importer.hpp>
#include "a_opengl_upload.hpp"
#include <assimp/postprocess.h>
using namespace Andromeda::ECS;
namespace Andromeda {
    void ResourceManager::start()
    {
        addMaterialShader<LitShader>(SHADER_PATH "unlitVertex.glsl", SHADER_PATH "unlitFragment.glsl");
        addMaterialShader<ColorShader>(SHADER_PATH "colorVertex.glsl", SHADER_PATH "colorFragment.glsl");
        addProceduralShader<GridShader>(SHADER_PATH "gridVertex.glsl", SHADER_PATH "gridFragment.glsl");
        addPostProcessShader<PostProcessShader>(SHADER_PATH "outlineVertex.glsl", SHADER_PATH "outlineFragment.glsl");
        addPostProcessShader<PostProcessShader>(SHADER_PATH "maskVertex.glsl", SHADER_PATH "maskFrag.glsl");
        loadModels();
        loadDeviceIcons();
        loadEditorIcons();
        setupMeshes();
        
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

    void ResourceManager::loadAllCubeMaps()
    {
        std::vector<std::string> directoryNames = Filesystem::getAllDirectoryNames(CUBEMAP_PATH);
        std::vector<std::string> skyboxPathsInDirectory;
        for (auto& directoryName : directoryNames)
        {
            m_CubemapData[directoryName] = CubemapData();
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

    u32 Andromeda::ResourceManager::getEditorIconID(const std::string& name)
    {
        auto it = m_EditorIcons.find(name);
        if (it != m_EditorIcons.end()) {
            return it->second.id;
        }
        throw std::runtime_error("Icon not found: " + name + "!");
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
        return m_GPUMeshes.at(meshID).vao;
    }

    u32 ResourceManager::getMeshIndexSizeByID(const u32 meshID) const {
        return m_DeviceRecords.at(meshID).mesh.m_Indexbuffer.size();
    }

    MaterialShader* ResourceManager::getMaterialShaderByID(MaterialShaderType t) const
    {
        return m_MaterialShaders[static_cast<int>(t)].get();
    }

    ProceduralShader* ResourceManager::getProceduralShaderByID(ProceduralShaderType t) const
    {
        return m_ProceduralShaders[static_cast<int>(t)].get();
    }

    PostProcessShader* ResourceManager::getPostprocessShaderByID(PostProcessShaderType t) const
    {
        return m_PostProcessShaders[static_cast<int>(t)].get();
    }

    const Mesh& ResourceManager::getMeshByID(const uint32_t meshID) const
    {
        return m_DeviceRecords.at(meshID).mesh;
    }

    u32 ResourceManager::getDeviceRecordsSize() const
    {
        return m_DeviceRecords.size();
    }

    const std::unordered_map<uint32_t, Device>& ResourceManager::getDeviceRecords() const
    {
        return m_DeviceRecords;
    }

    u32 ResourceManager::getDeviceCount() const {
        return static_cast<u32>(m_DeviceIndexList.size());
    }
    const Device& ResourceManager::getDeviceData(u32 index) const {
        u32 deviceID = m_DeviceIndexList.at(index);
        return m_DeviceRecords.at(deviceID);
    }

    u32 ResourceManager::getDeviceIconID(const deviceType type) const {
        return m_DeviceIcons.at(type).id;
    }
    
    void ResourceManager::loadCubemapTexture(CubemapData& data){
        i32 width, height, channels;
        for (u32 i = 0; i < data.faceTexturePath.size(); i++) {
            unsigned char* pixels = stbi_load(data.faceTexturePath[i], &width, &height, &channels, 0);
            data.pixelData[i] = pixels;
        }
    }

    void ResourceManager::loadAndStoreCubemap(const std::string& name, const std::array<const char*, 6>& paths) {
        CubemapData data;
        data.faceTexturePath = paths;
        loadCubemapTexture(data);

        m_CubemapData[name] = std::move(data);
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
        for (auto& [id, device] : m_DeviceRecords)
        {
            MeshGPUHandle& gpuHandle = m_GPUMeshes[id];
            createMesh(gpuHandle, device.mesh);
        }
    }
    void ResourceManager::processNode(const uint32_t meshId, const aiScene* scene, aiNode* node)
    {
        auto& newMesh = m_DeviceRecords.at(meshId).mesh;
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

            auto& vb = newMesh.m_Vertexbuffer;
            auto& ib = newMesh.m_Indexbuffer;

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

        auto [it, inserted] = m_DeviceRecords.try_emplace(
            id,
            Device{ id, meshName, Mesh{}, dt }
        );

        if (!inserted) {
            throw std::runtime_error("Failed to insert MeshRecord");
        }

        m_DeviceIndexList.push_back(id);
        m_MeshIDbyName.emplace(meshName, id);

        aiNode* rootNode = scene->mRootNode;
        processNode(id, scene, rootNode);
    }
}