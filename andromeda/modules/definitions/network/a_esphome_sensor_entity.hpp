#pragma once
#include <string>
namespace Andromeda{
    struct ESPHomeSensorEntity{
        std::string entity_id;
        std::string name;
        std::string type;
        std::string unit_of_measurement;
        std::string device_class;
        std::string state_class;
        std::string state;
        float state_parsed;
    };
}