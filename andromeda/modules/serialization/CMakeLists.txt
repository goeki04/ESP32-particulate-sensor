
find_package(nlohmann_json REQUIRED)

add_library(serialization STATIC
	sceneSerializer.cpp
)

target_include_directories(serialization PUBLIC .)


target_link_libraries(serialization PUBLIC
	nlohmann_json::nlohmann_json

	core
	definitions
)