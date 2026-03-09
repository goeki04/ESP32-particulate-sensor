
find_package(glm REQUIRED)
add_library(collision INTERFACE)

target_include_directories(collision INTERFACE .)

target_link_libraries(collision INTERFACE
	glm::glm
)