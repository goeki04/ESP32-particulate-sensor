#pragma once
namespace Andromeda {
    struct Vertex {
    public:
        glm::vec3 pos;
        glm::vec3 normal;
        glm::vec2 uv;
        glm::vec3 color; //this is not a vertex color, its the diffuse color
        Vertex() : pos(0.0f), normal(0.0f), uv(0.0f), color(1.0f) {};
    };
}