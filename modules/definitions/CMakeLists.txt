find_package(glm REQUIRED)
find_package(nlohmann_json REQUIRED)
add_library(definitions INTERFACE)
target_include_directories(definitions INTERFACE .)

target_link_libraries(definitions INTERFACE
	glm::glm
	nlohmann_json::nlohmann_json
)