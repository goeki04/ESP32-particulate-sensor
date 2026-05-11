#pragma once
#include "generated_undo_commands.hpp"
#include "a_primitives.hpp"

namespace Andromeda::ECS {
	class ComponentRegistry;
}
namespace Andromeda::Editor::Undo {
	class CommandDispatcher {
	public:
		static void dispatch(u8* rawData, ECS::ComponentRegistry& reg);
	};
}