find_package(asio REQUIRED)
add_library(network STATIC
    esphome_client.cpp
    esphome_decoder.cpp
)

target_include_directories(network PUBLIC .)

target_link_libraries(network PUBLIC asio::asio definitions)