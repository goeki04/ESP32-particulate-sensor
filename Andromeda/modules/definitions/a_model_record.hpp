#pragma once
#include "a_primitives.hpp"
#include <string>
namespace Andromeda {
	struct Mesh;

	enum class deviceType {
		DEFAULT = -1,
		SENSOR,
		CONTROLLER,
		CABLE,
		BREADBOARD,
	};

	struct ModelRecord {
		u32 meshID;
		std::string name;
		deviceType type;
	};

	class IModelProvider {
	public:
		virtual ~IModelProvider() = default;
		virtual u32 getModelCount() const = 0;
		virtual const ModelRecord& getModelData(u32 index) const = 0;
		virtual u32 getDeviceIconID(deviceType type) const = 0;
	};
}