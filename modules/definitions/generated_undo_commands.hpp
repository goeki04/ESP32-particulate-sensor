#pragma once
#include "a_components.hpp"
#include <cstdint>

namespace Andromeda::Editor::Undo {

enum class CommandType : uint16_t {
    None = 0,
    UpdateTag,
    UpdateTransform,
};

struct UndoTagData {
    uint64_t entityID;
    ECS::Component::Tag oldState;
    ECS::Component::Tag newState;
};

struct UndoTransformData {
    uint64_t entityID;
    ECS::Component::Transform oldState;
    ECS::Component::Transform newState;
};

}