#pragma once
#include "a_components.hpp"
#include <cstdint>
#include "a_primitives.hpp"

namespace Andromeda::Editor::Undo {

enum class CommandType : uint16_t {
    UpdateTag = 0,
    UpdateTransform = 1,
    COUNT = 2
};
struct CommandHeader {
    CommandType type;
    u32 dataSize;
};

struct UndoTagData {
    ECS::Entity entityID;
    ECS::Component::Tag oldState;
    ECS::Component::Tag newState;
};

struct UndoTransformData {
    ECS::Entity entityID;
    ECS::Component::Transform oldState;
    ECS::Component::Transform newState;
};

}
