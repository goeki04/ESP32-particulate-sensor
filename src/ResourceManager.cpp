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
    loadScene("../assets/models/ESP32Wroom.fbx");
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
//TODO: TEXTURE CACHING
void ResourceManager::processNode(const aiScene* scene,aiNode* node, aiMatrix4x4 parentTransform)
{
    aiMatrix4x4 globalTransform = parentTransform * node->mTransformation;
    for (int i = 0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        std::vector<unsigned int> indexBuffer;
        std::vector<Vertex> vertices;

        vertices.reserve(mesh->mNumVertices);
        aiMatrix3x3 normalMatrix = aiMatrix3x3(globalTransform);
        normalMatrix = normalMatrix.Inverse().Transpose();
        for (int j = 0; j < mesh->mNumVertices; j++) {
            Vertex vertex{ };
            aiVector3D worldPos = mesh->mVertices[j];
            vertex.setPosition(worldPos.x, worldPos.y, worldPos.z);
            if (mesh->HasNormals()) {
                aiVector3D n = mesh->mNormals[j];
                aiVector3D worldNormal = normalMatrix * n;
                worldNormal.Normalize();
                vertex.setNormals(worldNormal.x, worldNormal.y, worldNormal.z);
            }
            else {
                std::printf("Vertex doesnt have normals");
            }
            if (mesh->HasTextureCoords(0)) {

                vertex.setUV(mesh->mTextureCoords[0][j].x, mesh->mTextureCoords[0][j].y);
            }
            else {
                std::cout << "Mesh doesn't have UV-coordinates. You should fix that." << std::endl;
            }
            vertices.push_back(vertex);
        }

        for (int k = 0; k < mesh->mNumFaces; k++) {
            indexBuffer.insert(indexBuffer.end(), mesh->mFaces[k].mIndices, mesh->mFaces[k].mIndices + mesh->mFaces[k].mNumIndices);
        }
        Mesh newMesh;
        newMesh.m_VertexBuffer = std::move(vertices);
        newMesh.m_IndexBuffer = std::move(indexBuffer);
        aiColor3D color(1.0f, 1.0f, 1.0f);
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        aiString texPath;
        if (material->Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS) {
            m_Textures.emplace_back(createColorTexture(color));
        }
        if (material->Get(AI_MATKEY_COLOR_DIFFUSE, color) != AI_SUCCESS)
        {
            // falls das Material KEINE diffuse Farbe hat, eine Default-Textur erzeugen
            m_Textures.emplace_back(createColorTexture(color));
        }
        newMesh.setTextureID(m_Textures[m_Textures.size() - 1]);
        m_Meshes.push_back(std::move(newMesh));
    }
    for (int i = 0; i < node->mNumChildren;i++) {
        processNode(scene,node->mChildren[i], globalTransform);
    }
}

void ResourceManager::loadScene(const char* path) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals);
    if (scene == nullptr) {
        throw std::runtime_error(importer.GetErrorString());
    }
    aiNode* rootNode = scene->mRootNode;
    aiMatrix4x4 identity(
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    );
    processNode(scene,rootNode,identity);
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
