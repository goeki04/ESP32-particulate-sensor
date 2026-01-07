#include "pch.h"
#include "ResourceManager.h"
#include "SubsystemManager.h"
#include "WindowManager.h"

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
    loadScene("../assets/models/ESP32Wroom.fbx");
    loadScene("../assets/models/BMV080.obj");
    
    setupMeshes(); 
    m_SceneObjects[0].m_Transform.rotation = glm::vec3(0, 0.0f, 0);
    m_SceneObjects[1].m_Transform.rotation = glm::vec3(glm::radians(180.0f), 0.0f, 0.0f);
    for (auto& s : m_SceneObjects) {
        s.initialize();
    }
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
    for (auto& sceneObject : m_SceneObjects) {
        sceneObject.m_Mesh.createMesh();
        sceneObject.setShader(m_Shaders[(int)shaderType::unlit].get());
    }
}
//TODO: texture caching
void ResourceManager::processNode(const std::string& path,const aiScene* scene, aiNode* node)
{
    SceneObject* sceneObject = nullptr;

    // Nur Nodes mit Meshes bekommen ein SceneObject
    if (node->mNumMeshes > 0) {
        m_SceneObjects.emplace_back();
        sceneObject = &m_SceneObjects.back();
    }

    if (sceneObject) {
        auto& newMesh = sceneObject->m_Mesh;

        for (unsigned int i = 0; i < node->mNumMeshes; i++) {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            aiColor3D color(1.0f, 1.0f, 1.0f);
            aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

            std::vector<unsigned int> indexBuffer;
            std::vector<Vertex> vertices;
            vertices.reserve(mesh->mNumVertices);

            bool hasNormals = mesh->HasNormals();
            if (!hasNormals){
                std::printf("Vertex doesnt have normals");
            }

            for (unsigned int j = 0; j < mesh->mNumVertices; j++) {
                Vertex vertex{};
                aiVector3D worldPos = mesh->mVertices[j];
                vertex.pos = glm::vec3(worldPos.x, worldPos.y, worldPos.z);

                if (hasNormals) {
                    vertex.normal = glm::vec3(mesh->mNormals[j].x, mesh->mNormals[j].y, mesh->mNormals[j].z);
                }

                if (mesh->HasTextureCoords(0)) {
                    vertex.uv = glm::vec2(mesh->mTextureCoords[0][j].x,mesh->mTextureCoords[0][j].y);
                }
                else {
                    std::cout << "Mesh doesn't have UV-coordinates. You should fix that." << std::endl;
                }
                if (material->Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS) {
                    vertex.color = glm::vec3(color.r, color.g,color.b);
                }
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
    }
    for (int i = 0; i < node->mNumChildren; i++) {
        processNode(path,scene, node->mChildren[i]);
    }
}

void ResourceManager::loadScene(const std::string& path) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path.c_str(), aiProcess_Triangulate | aiProcess_ConvertToLeftHanded | aiProcess_FlipUVs | aiProcess_GenNormals);
    if (scene == nullptr) {
        throw std::runtime_error(importer.GetErrorString());
    }
    if (m_Meshes.find(path) != m_Meshes.end()) {
        std::printf("Mesh already exists!");
        return;
    }
    //Create new SceneObject
    size_t namePos = path.find_last_of("/\\");
    std::string objectName = path.substr(namePos+1);
    size_t fileExtPos = objectName.find_last_of(".");
    m_Meshes.emplace(objectName.substr(0,fileExtPos), Mesh());

    aiNode* rootNode = scene->mRootNode;
    processNode(path,scene,rootNode);
}
