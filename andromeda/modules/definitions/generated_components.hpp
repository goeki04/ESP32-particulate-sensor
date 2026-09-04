#pragma once

#include <tuple>

namespace Andromeda::ECS::Component {
    struct AABB;
    struct Device;
    struct Material;
    struct MeshRenderer;
    struct ParticleSystem;
    struct Tag;
    struct Transform;

    using ComponentDirectory = std::tuple<
        AABB,
        Device,
        Material,
        MeshRenderer,
        ParticleSystem,
        Tag,
        Transform
    >;
}
