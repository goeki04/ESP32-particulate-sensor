#include "pch.h"
#include "ResourceManager.h"
#include "SubsystemManager.h"
#include "WindowManager.h"
#include "SceneObject.h"
#include "util.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
void ResourceManager::start()
{
    auto windowManager = SystemManager::getInstance().getSubsystem<Window::WindowManager>();
    m_GlContext = SDL_GL_CreateContext(Window::g_Window);
    if (!m_GlContext) {
        throw std::exception("Failed to create SDL_GL context!");
    }
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        throw std::exception("Failed to call glewInit!");
    }
    addShader<UnlitShader>("../src/shader/vertexShader.glsl", "../src/shader/fragmentShader.glsl");
    loadModels();
    loadIcons();
    setupMeshes(); 
    getAllFilesInDirectory("hello there");
}

void ResourceManager::update() {
    m_Cam.setProjectionMatrix(GuiManager::s_ViewportSize.x, GuiManager::s_ViewportSize.y);
}

void ResourceManager::loadModels()
{
    std::string filter[2];
    filter[0] = ".fbx";
    filter[1] = ".obj";
    std::vector<std::string> paths = getAllFilesInDirectoryRecursive("../assets/models",filter);

    for (auto& v : paths) {
        loadScene(v);
    }
}

void ResourceManager::loadIcons()
{
    std::string filter[2];
    filter[0] = ".png";
    filter[1] = ".jpg";
    std::vector<std::string> paths = getAllFilesInDirectory("../assets/icons", filter);

    for (auto& v : paths) {
        std::string fileName = getFileName(v);
        m_DeviceIcons[fileName] = ResourceManager::CreateSDLSurface(v.c_str());
    }
}

std::string ResourceManager::getFileName(const std::string& path) const
{
    size_t namePos = path.find_last_of("/");
    std::string objectName = (namePos == std::string::npos) ? path : path.substr(namePos + 1);

    size_t dotPos = objectName.find_last_of('.');
    std::string fileName = (dotPos == std::string::npos) ? objectName : objectName.substr(0, dotPos);
    return fileName;
}

std::vector<std::string> ResourceManager::getAllFilesInDirectory(const std::string& directory)
{
    std::vector<std::string> filePaths;
    std::error_code ec;
    for (const auto& entry : std::filesystem::directory_iterator(directory,ec)) {
        if (ec) break;
        if (!entry.is_regular_file()) continue;
        filePaths.emplace_back(entry.path().generic_string());
    }
    return filePaths;
}

std::vector<std::string> ResourceManager::getAllFilesInDirectory(const std::string& directory,std::span<std::string> filter)
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

std::vector<std::string> ResourceManager::getAllFilesInDirectoryRecursive(const std::string& directory, std::span<const std::string> filter)
{
    std::vector<std::string> filePaths;
    std::error_code ec;

    std::filesystem::recursive_directory_iterator it(directory,std::filesystem::directory_options::skip_permission_denied,ec);

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

void ResourceManager::updateEvent(SDL_Event* event) {
    if (GuiManager::s_ViewportFocused)
    {
        m_Cam.cameraMovement();
        m_Cam.zoom(event);
    }
}
SDL_Surface* ResourceManager::CreateSDLSurface(const char* path)
{
    int w, h, channels;
    unsigned char* pixels = stbi_load(path, &w, &h, &channels, 4);
    if (!pixels) {
        SDL_Log("Failed to load PNG: %s", stbi_failure_reason());
        return NULL;
    }
    SDL_Surface* surface = SDL_CreateSurfaceFrom(w, h, SDL_PIXELFORMAT_RGBA32, pixels, w * 4);
    if (!surface) {
        stbi_image_free(pixels);
        SDL_Log("Failed to create surface: %s", SDL_GetError());
        return NULL;
    }
    return surface;
}

void ResourceManager::addSceneObject(const std::string& name, unsigned int meshID)
{
    m_SceneObjects.emplace_back(this, meshID, m_NextSceneObjectID, name);
    m_NextSceneObjectID++;
}

void ResourceManager::deleteSceneObject(SceneObject& sceneObject)
{
    m_SceneObjects.erase(std::remove(m_SceneObjects.begin(), m_SceneObjects.end(), sceneObject), m_SceneObjects.end());
}

GLsizei ResourceManager::getMeshVaoByID(uint32_t meshID)
{
    return m_DeviceRecords.at(meshID).mesh.m_Vao;
}

GLsizei ResourceManager::getMeshIndexSizeByID(uint32_t meshID) {
    return m_DeviceRecords.at(meshID).mesh.m_IndexBuffer.size();
}

Shader* ResourceManager::getShaderByID(shaderType type)
{
    return m_Shaders[(int)type].get();
}

Mesh& ResourceManager::getMeshByID(uint32_t meshID)
{
    return m_DeviceRecords.at(meshID).mesh;
}

size_t ResourceManager::getMeshRecordsSize()
{
    return m_DeviceRecords.size();
}

void ResourceManager::setupMeshes()
{
    for (auto it = m_DeviceRecords.begin(); it != m_DeviceRecords.end(); ++it) {
        it->second.mesh.createMesh();
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
            else {
                std::cout << "Mesh doesn't have UV-coordinates. You should fix that." << std::endl;
            }
            vertex.color = glm::vec3(color.r,color.g,color.b);
            vertices.push_back(vertex);
        }

        auto& vb = newMesh.m_VertexBuffer;
        auto& ib = newMesh.m_IndexBuffer;

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

/// <summary>
/// This function sets the device type based on the path.
/// Sets the device type to default if not found.
/// </summary>
/// <param name="path">path which used to determine the device type based on the directory</param>
/// <returns></returns>
deviceType ResourceManager::findDeviceTypeByPath(const std::string& path) {
    std::string directory = path;
    std::cout << path;
    util::stringToLower(directory);
    if (directory == "dsensor") return deviceType::SENSOR;
    if (directory == "dcontroller") return deviceType::CONTROLLER;
    if (directory == "dcable") return deviceType::CABLE;
    if (directory == "dbreadboard") return deviceType::BREADBOARD;
    std::printf("[WARNING]: Device type not found");
    return deviceType::DEFAULT;
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
    std::string meshName = getFileName(path);
    findDeviceTypeByPath(path);
    if (m_MeshIDbyName.contains(meshName)) {
        std::printf("Mesh already exists!\n");
        return;
    }
    const uint32_t id = m_NextMeshID++;
    auto [it, inserted] = m_DeviceRecords.try_emplace(
        id,
        Device{ id, meshName, Mesh{} }
    );
    if (!inserted) {
        throw std::runtime_error("Failed to insert MeshRecord");
    }

    m_MeshIDbyName.emplace(meshName, id);

    aiNode* rootNode = scene->mRootNode;

    processNode(id, scene, rootNode);
}
