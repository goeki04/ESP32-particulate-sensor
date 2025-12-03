#include "pch.h"
#include "ResourceManager.h"
#include "SubsystemManager.h"
#include "WindowManager.h"

void ResourceManager::start()
{
    auto windowManager = SystemManager::getInstance().getSubsystem<WindowManager>();
    m_GlContext = SDL_GL_CreateContext(windowManager->m_Window);
    if (!m_GlContext) {
        throw std::exception("Failed to create SDL_GL context!");
    }
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        throw std::exception("Failed to call glewInit!");
    }
    addShader<UnlitShader>("../src/shader/vertexShader.glsl", "../src/shader/fragmentShader.glsl");
    loadAssimpScene("../assets/models/cube.obj");
    setupMeshes();
}
void ResourceManager::setupMeshes()
{
    for (auto& mesh : m_Meshes) {
        mesh.createMesh();
        mesh.setShader(m_Shaders[(int)shaderType::unlit].get());
    }
}

/// <summary>
/// Call this function before ImGui::Render()
/// </summary>
void ResourceManager::loadAssimpScene(const char* path)
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
    if (scene == nullptr) {
        throw std::runtime_error(importer.GetErrorString());
    }
    int numMeshes = scene->mNumMeshes; //Todo: what happens when this function is called multiple times?
    m_Meshes.resize(numMeshes);
    for (int i = 0; i < numMeshes; i++) {
        std::vector<unsigned int> indexBuffer;
        std::vector<Vertex> vertices;
        aiMesh* mesh = scene->mMeshes[i];
        vertices.reserve(mesh->mNumVertices);
        for (int j = 0; j < mesh->mNumVertices; j++) {
            vertices.emplace_back(mesh->mVertices[j].x, mesh->mVertices[j].y, mesh->mVertices[j].z);
        }
        for (int k = 0; k < mesh->mNumFaces; k++) {
            indexBuffer.insert(indexBuffer.end(), mesh->mFaces[k].mIndices, mesh->mFaces[k].mIndices + mesh->mFaces[k].mNumIndices);
        }
        m_Meshes[i].m_VertexBuffer = std::move(vertices);
        m_Meshes[i].m_IndexBuffer = std::move(indexBuffer);
    }
}
