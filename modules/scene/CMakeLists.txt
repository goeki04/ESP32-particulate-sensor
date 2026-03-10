
add_library(scene STATIC
	scene.cpp
)

target_include_directories(scene PUBLIC .)


target_link_libraries(scene PUBLIC
	core
	definitions
)