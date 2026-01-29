#pragma once
#include "Shader.h"
#include "components.h"
class ResourceManager;
namespace ECS {
    namespace system {
        class MeshRenderingSystem {
            void drawMesh(ResourceManager* rm,component::Mesh& mesh, component::Transform& transform);
        };

        class PickingSystem {

        };

    }
}