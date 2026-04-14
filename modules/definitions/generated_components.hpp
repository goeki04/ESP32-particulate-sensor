#pragma once

#include <tuple>

namespace Andromeda::ECS::Component {
    struct AABB;
    struct Mesh;
    struct Tag;
    struct Transform;

    using ComponentDirectory = std::tuple<
        AABB,
        Mesh,
        Tag,
        Transform
    >;
}
