#pragma once
#include <vector>
#include "a_primitives.hpp"
#include "generated_undo_commands.hpp"

namespace Andromeda::ECS {
	class ComponentRegistry;
}
namespace Andromeda::Editor::Undo {

	class UndoBuffer {
	private:
		std::vector<u8> data;
		std::vector<size_t> commandOffsets;
		const size_t maxUndoSteps = 50;
	public:

		void appendBytes(const void* src, size_t size);
		void deleteCommand();
		void undo(ECS::ComponentRegistry& registry);
		template<typename T>
		void pushCommand(CommandType cmdType, const T& commandData) {
			if (commandOffsets.size() >= maxUndoSteps) {
				deleteCommand();
			}

			commandOffsets.push_back(data.size());

			CommandHeader header{ cmdType, (u32)sizeof(T) };
			appendBytes(&header, sizeof(CommandHeader));

			appendBytes(&commandData, sizeof(T));
		}
	};
}