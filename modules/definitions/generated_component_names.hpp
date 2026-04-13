#pragma once
#include <string_view>
#include "a_components.hpp"

namespace Andromeda::ECS::Component {

    template<typename T>
    constexpr std::string_view get_component_name() { return "Unknown"; }

    template<> constexpr std::string_view get_component_name<AABB>() { return "AABB"; }
    template<> constexpr std::string_view get_component_name<Mesh>() { return "Mesh"; }
    template<> constexpr std::string_view get_component_name<Selected>() { return "Selected"; }
    template<> constexpr std::string_view get_component_name<Tag>() { return "Tag"; }
    template<> constexpr std::string_view get_component_name<Transform>() { return "Transform"; }

}
