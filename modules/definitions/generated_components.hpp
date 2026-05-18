#pragma once

#include <tuple>

namespace Andromeda::ECS::Component {
    struct AABB;
    struct Device;
    struct MeshRenderer;
    struct Tag;
    struct Transform;

    using ComponentDirectory = std::tuple<
        AABB,
        Device,
        MeshRenderer,
        Tag,
        Transform
    >;
}
