find_package(glm REQUIRED)

add_library(definitions INTERFACE)
target_include_directories(definitions INTERFACE .)

target_link_libraries(definitions INTERFACE
	glm::glm
)