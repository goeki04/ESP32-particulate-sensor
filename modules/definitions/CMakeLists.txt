find_package(glm REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(Python3 REQUIRED COMPONENTS Interpreter)
set(GEN_SCRIPT "${CMAKE_SOURCE_DIR}/tools/gen_components.py")
set(GEN_OUTPUT "${CMAKE_CURRENT_SOURCE_DIR}/generated_components.hpp")

add_custom_command(
    OUTPUT ${GEN_OUTPUT}
    COMMAND ${Python3_EXECUTABLE} ${GEN_SCRIPT}
    DEPENDS ${GEN_SCRIPT}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Scanning modules for a_components.hpp to sync Tuple..."
    VERBATIM
)
add_custom_target(generate_ecs_components DEPENDS ${GEN_OUTPUT})
add_library(definitions INTERFACE)
add_dependencies(definitions generate_ecs_components)

target_sources(definitions INTERFACE ${GEN_OUTPUT})
target_include_directories(definitions INTERFACE .)

target_link_libraries(definitions INTERFACE
	glm::glm
	nlohmann_json::nlohmann_json
)