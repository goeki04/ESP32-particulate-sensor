
find_package(Boost REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(httplib REQUIRED)
add_library(network STATIC
    a_HomeAssistant.cpp
    a_network_manager.cpp
    service/a_WeatherService.cpp
    a_BoostWebsocketClient.cpp
    
)

target_compile_definitions(network PRIVATE SOURCE_DIRECTORY="${CMAKE_SOURCE_DIR}")

target_include_directories(network PUBLIC .)

target_link_libraries(network PUBLIC
nlohmann_json::nlohmann_json
httplib::httplib
OpenSSL::SSL
Boost::boost
filesystem

core
)