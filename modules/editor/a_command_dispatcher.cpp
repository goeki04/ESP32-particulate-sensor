#include "a_command_dispatcher.hpp"
#include "a_registry.hpp"
#include "generated_undo_commands.hpp"

	namespace Andromeda::Editor::Undo {
		#include "generated_dispatcher.inc"
		void CommandDispatcher::dispatch(u8* rawData, ECS::ComponentRegistry& reg)
		{
			CommandHeader* header = reinterpret_cast<CommandHeader*>(rawData);

			u8* payload = rawData + sizeof(CommandHeader);

			auto index = static_cast<size_t>(header->type);

			assert(index < static_cast<size_t>(CommandType::Count));
			if (index < static_cast<uint16_t>(CommandType::COUNT)) {
				g_undoDispatchTable[index](payload, reg);
			}
		}
	}