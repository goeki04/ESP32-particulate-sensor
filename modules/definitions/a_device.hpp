#pragma once
namespace Andromeda {
	enum class deviceType {
		DEFAULT = -1,
		SENSOR,
		CONTROLLER,
		CABLE,
		BREADBOARD,
	};

	struct Device {
		uint32_t id;
		std::string name;
		Mesh mesh;
		deviceType type;
	};
}