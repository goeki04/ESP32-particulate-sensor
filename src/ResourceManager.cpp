#include "ResourceManager.h"

void ResourceManager::start()
{
    loadAssimpScene("../assets/models/cube.obj");
}

void ResourceManager::loadImages()
{
}


void ResourceManager::setupShaders()
{
   /* for (int i = 0; i < m_Shaders.size(); i++) {
        m_Shaders[i].compileShader();
    }*/
}
void ResourceManager::setupMeshes()
{
    for (int i = 0; i < m_Meshes.size(); i++) {
        m_Meshes[i].createMesh();
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
