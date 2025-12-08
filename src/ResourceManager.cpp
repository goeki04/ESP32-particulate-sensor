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
    loadAssimpScene("../assets/models/scene.gltf");
    setupMeshes();
}

void ResourceManager::update() {
    m_Cam.setProjectionMatrix(GuiManager::s_ViewportSize.x, GuiManager::s_ViewportSize.y);
}

void ResourceManager::updateEvent(SDL_Event* event) {
    if (GuiManager::s_ViewportFocused)
    {
        m_Cam.cameraMovement();
        m_Cam.zoom(event);
    }
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
            vertices.emplace_back();
            vertices[j].setPosition(mesh->mVertices[j].x, mesh->mVertices[j].y, mesh->mVertices[j].z);
            if (mesh->HasNormals()) {
                vertices[j].setNormals(mesh->mNormals[j].x, mesh->mNormals[j].y, mesh->mNormals[j].z);
            }
            vertices[j].setUV(mesh->mTextureCoords[0][j].x, mesh->mTextureCoords[0][j].y);
        }

        for (int k = 0; k < mesh->mNumFaces; k++) {
            indexBuffer.insert(indexBuffer.end(), mesh->mFaces[k].mIndices, mesh->mFaces[k].mIndices + mesh->mFaces[k].mNumIndices);
        }

        m_Meshes[i].m_VertexBuffer = std::move(vertices);
        m_Meshes[i].m_IndexBuffer = std::move(indexBuffer);
        aiColor3D color(1.0f, 1.0f, 1.0f);
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        aiString texPath;
        if (material->Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS) {
                m_Textures.emplace_back(createColorTexture(color));
        }
        m_Meshes[i].setTextureID(m_Textures[m_Textures.size() - 1]);
    }
}

GLuint ResourceManager::createColorTexture(aiColor3D& color)
{
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    unsigned char data[3] = {
        static_cast<unsigned char>(color.r * 255),
        static_cast<unsigned char>(color.g * 255),
        static_cast<unsigned char>(color.b * 255)
    };

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    return textureID;
}
