find_package(asio REQUIRED)
add_library(network STATIC
    ESPHomeClient.cpp
    EspHomeDecoder.cpp
)

target_include_directories(network PUBLIC .)

target_link_libraries(network PUBLIC asio::asio)