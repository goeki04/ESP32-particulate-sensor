#pragma once
#include "a_geometry.hpp"
#include "a_primitives.hpp"
namespace Andromeda {
	enum class deviceType {
		DEFAULT = -1,
		SENSOR,
		CONTROLLER,
		CABLE,
		BREADBOARD,
	};

	struct Device {
		u32 id;
		std::string name;
		Mesh mesh;
		deviceType type;
	};

	class IDeviceProvider {
	public:
		virtual ~IDeviceProvider() = default;
		virtual u32 getDeviceCount() const = 0;
		virtual const Device& getDeviceData(u32 index) const = 0;
		virtual u32 getDeviceIconID(deviceType type) const = 0;
	};
}