#include "a_undo_buffer.hpp"
#include "a_command_dispatcher.hpp"
#include "a_registry.hpp"
namespace Andromeda::Editor::Undo {

	void UndoBuffer::appendBytes(const void* src, size_t size)
	{
		const u8* bytePtr = reinterpret_cast<const u8*>(src);

		data.insert(data.end(), bytePtr, bytePtr+size);
	}
	void UndoBuffer::deleteCommand()
	{
		if(commandOffsets.empty()) return;
		size_t firstCommandSize;
		if (commandOffsets.size() > 1) {
			firstCommandSize = commandOffsets[1];
		}
		else {
			firstCommandSize = data.size();
		}
		data.erase(data.begin(), data.begin() + firstCommandSize);

		commandOffsets.erase(commandOffsets.begin());

		for (size_t& offset : commandOffsets) {
			offset -= firstCommandSize;
		}
	}
	void UndoBuffer::undo(ECS::ComponentRegistry& reg)
	{
		if(commandOffsets.empty()) return;

		size_t lastOffset = commandOffsets.back();

		u8* lastCommandPtr = &data[lastOffset];

		CommandDispatcher::dispatch(lastCommandPtr, reg);

		data.resize(lastOffset);
		commandOffsets.pop_back();
	}
}