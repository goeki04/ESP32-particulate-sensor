find_package(asio REQUIRED)
find_package(ixwebsocket REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(httplib REQUIRED)
add_library(network STATIC
    esphome_client.cpp
    esphome_decoder.cpp
    a_HomeAssistant.cpp
    service/a_WeatherService.cpp
)

target_compile_definitions(network PRIVATE SOURCE_DIRECTORY="${CMAKE_SOURCE_DIR}")

target_include_directories(network PUBLIC .)

target_link_libraries(network PUBLIC
asio::asio
ixwebsocket::ixwebsocket
nlohmann_json::nlohmann_json
httplib::httplib
OpenSSL::SSL
definitions
filesystem
)