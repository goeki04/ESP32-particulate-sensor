#pragma once

/**
 * @file a_command_dispatcher.hpp
 * @brief Decodes a serialized undo command and applies it to the ECS registry.
 */

#include "generated_undo_commands.hpp"
#include "a_primitives.hpp"

namespace Andromeda::ECS {
	class ComponentRegistry;
}
namespace Andromeda::Editor::Undo {
	/**
	 * @class CommandDispatcher
	 * @brief Interprets a raw command record from the @c UndoBuffer and performs the corresponding revert.
	 *
	 * @details Acts as the read side of the data-oriented undo system: it reads the
	 *          @c CommandHeader at the front of @p rawData to determine the command type, casts
	 *          the trailing bytes to the matching payload struct, and writes the stored "old state"
	 *          back into the registry.
	 *
	 * @see UndoBuffer
	 */
	class CommandDispatcher {
	public:
		/**
		 * @brief Decodes and applies a single serialized undo command.
		 * @param rawData Pointer to the command record (a @c CommandHeader followed by its payload).
		 * @param reg The component registry the decoded command is applied to.
		 */
		static void dispatch(u8* rawData, ECS::ComponentRegistry& reg);
	};
}