#include "Renderer.h"
#include "pch.h"
#include "SubsystemManager.h"
#include "WindowManager.h"

const std::string modelPath = "../assets/models/cube.obj";
void Renderer::start()
{
	m_WindowManager = SystemManager::getInstance().getSubsystem<WindowManager>();
    SDL_GLContext glContext = SDL_GL_CreateContext(m_WindowManager->m_Window);
    if (!glContext) {
        throw std::exception("Failed to create SDL_GL context!");
    }
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        throw std::exception("Failed to call glewInit!");
    }
    m_GuiManager.init(m_WindowManager->m_Window);
    ImVec2 viewportWindowSize = m_GuiManager.getViewportWindowSize();
    m_framebufferSize = glm::ivec2(viewportWindowSize.x, viewportWindowSize.y);
    ImGui_ImplSDL3_InitForOpenGL(m_WindowManager->m_Window, glContext);
    ImGui_ImplOpenGL3_Init(Renderer::glsl_version);
    createFramebuffer();
    loadObjModel(modelPath);
}

/// <summary>
/// Call this function before ImGui::Render()
/// </summary>


void Renderer::loadObjModel(const std::string& path)
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
    if (scene == nullptr) {
        throw std::runtime_error(importer.GetErrorString());
    }
    int numMeshes = scene->mNumMeshes;
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
        m_Meshes[i].vertexBuffer = std::move(vertices);
        m_Meshes[i].indexBuffer = std::move(indexBuffer);
        std::cout << m_Meshes[i].vertexBuffer.size();
    }
}

void Renderer::update()
{
    m_GuiManager.update();
    m_GuiManager.drawViewportGUI(m_FramebufferTexture, ImVec2(m_framebufferSize.x,m_framebufferSize.y));
    ImGui::Render();
    glViewport(0, 0, m_WindowManager->m_WindowWidth, m_WindowManager->m_WindowHeight);
    glClearColor(0.172f, 0.172f, 0.329f,1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glBindFramebuffer(GL_FRAMEBUFFER,m_Framebuffer);
    glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glBindFramebuffer(GL_FRAMEBUFFER,0);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    SDL_GL_SwapWindow(m_WindowManager->m_Window);
}
/// <summary>
/// Call this function after initializing guimanager to create an offscreen framebuffer.
/// </summary>
void Renderer::createFramebuffer()
{
    glGenFramebuffers(1, &m_Framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_Framebuffer);
    //Color attachment
    glGenTextures(1, &m_FramebufferTexture);
    glBindTexture(GL_TEXTURE_2D, m_FramebufferTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_framebufferSize.x, m_framebufferSize.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_FramebufferTexture, 0);
    //Depth and stencil information
    glGenRenderbuffers(1, &m_Rendererbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, m_Rendererbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_framebufferSize.x, m_framebufferSize.y);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_Rendererbuffer);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Framebuffer is not complete";
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::destroy() {
    glDeleteTextures(1,&m_FramebufferTexture);
    glDeleteRenderbuffers(1,&m_Rendererbuffer);
    glDeleteFramebuffers(1,&m_Framebuffer);
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();
}
#include <sstream>
const char* Renderer::readShaderSource(const char* shaderPath)
{
    std::ifstream fileStream(shaderPath);
    std::stringstream buffer;
    buffer << fileStream.rdbuf();
    std::string shaderSource = buffer.str();
    const char* cShaderSource = shaderSource.c_str();
    buffer.clear();
    fileStream.close();
    return cShaderSource;
}
void Renderer::compileDefaultShader(const char* vertexShaderPath, const char* fragmentShaderPath)
{
    GLint success;

    const char* vertexShaderSource = readShaderSource(vertexShaderPath);
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1,&vertexShaderSource,NULL);
    glCompileShader(vertexShader);

    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        throw std::runtime_error("failed compiling vertex shader!");
    }
    const char* fragmentShaderSource = readShaderSource(fragmentShaderPath);
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1,&fragmentShaderSource,NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        throw std::runtime_error("failed compiling fragment shader!");
    }
    m_DefaultShader = glCreateProgram();
    glAttachShader(m_DefaultShader,vertexShader);
    glAttachShader(m_DefaultShader, fragmentShader);
    glLinkProgram(m_DefaultShader);
    glGetProgramiv(m_DefaultShader, GL_LINK_STATUS, &success);
    if (!success) {
        throw std::runtime_error("failed to create a shader program!");
    }
}
