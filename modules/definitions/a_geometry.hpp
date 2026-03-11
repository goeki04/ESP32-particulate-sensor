#pragma once
#include <vector>
#include <glm/glm.hpp>          
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
namespace Andromeda {
    struct Vertex {
    public:
        glm::vec3 pos;
        glm::vec3 normal;
        glm::vec2 uv;
        glm::vec3 color; //this is not a vertex color, its the diffuse color
        Vertex() : pos(0.0f), normal(0.0f), uv(0.0f), color(1.0f) {};
    };
    struct AABB {
        glm::vec3 min{ 1e34f };
        glm::vec3 max{ -1e34f };
    };

    struct MeshData {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        AABB localAABB;
    };
}