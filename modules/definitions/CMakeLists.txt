find_package(glm REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(Python3 REQUIRED COMPONENTS Interpreter)
set(GEN_SCRIPT "${CMAKE_SOURCE_DIR}/metaData/gen_components.py")
set(GEN_OUTPUT "${CMAKE_CURRENT_SOURCE_DIR}/ECS/generated_components.hpp")
set(GEN_NAME_SCRIPT "${CMAKE_SOURCE_DIR}/metaData/gen_components_name.py")
set(GEN_NAME_OUTPUT "${CMAKE_CURRENT_SOURCE_DIR}/ECS/generated_component_names.hpp")
set(COMPONENTS
    ECS/a_components.hpp
)
add_custom_command(
    OUTPUT ${GEN_OUTPUT}
    COMMAND ${Python3_EXECUTABLE} ${GEN_SCRIPT}
    DEPENDS ${GEN_SCRIPT} ${COMPONENTS}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Scanning modules for a_components.hpp to sync Tuple..."
    VERBATIM
)
add_custom_command(
    OUTPUT ${GEN_NAME_OUTPUT}
    COMMAND ${Python3_EXECUTABLE} ${GEN_NAME_SCRIPT}
    DEPENDS ${GEN_NAME_SCRIPT} ${GEN_OUTPUT}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    VERBATIM
)
add_custom_target(generate_ecs_metadata DEPENDS ${GEN_OUTPUT} ${GEN_NAME_OUTPUT})

add_library(definitions INTERFACE
		Math/a_glm_json_parser.hpp)
add_dependencies(definitions generate_ecs_metadata)
target_sources(definitions INTERFACE ${GEN_OUTPUT} ${GEN_NAME_OUTPUT})
target_include_directories(definitions INTERFACE 
   .   
   Math
   Engine
   ECS
)

target_link_libraries(definitions INTERFACE
	glm::glm
	nlohmann_json::nlohmann_json
)