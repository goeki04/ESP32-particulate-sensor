#include <iostream>
#include <vector>
#include <string>
#include <WinSock2.h>
#include <Windows.h>
#include <WS2tcpip.h>
#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>

#pragma comment(lib, "Ws2_32.lib")

using json = nlohmann::json;

constexpr int32_t PORT = 8080;
constexpr int BUFFER_SIZE = 1024;

int main() {
    WSADATA wsaData;
    int wsa_result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (wsa_result != 0) {
        spdlog::critical("WSAStartup failed with error: {}", wsa_result);
        return 1;
    }

    SOCKET server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (server_socket == INVALID_SOCKET) {
        spdlog::critical("Socket creation failed with error: {}", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    char opt = 1;
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET; // af_inet -> request ipv4 endpoint from the os
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT); // htons converts the port number to network byte order (big endian)

    if (bind(server_socket, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr)) == SOCKET_ERROR) {
        spdlog::critical("Bind failed with error: {}", WSAGetLastError());
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }

    if (listen(server_socket, SOMAXCONN) == SOCKET_ERROR) {
        spdlog::critical("Listen failed with error: {}", WSAGetLastError());
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }

    spdlog::info("Server is listening on port {}...", PORT);

    std::vector<SOCKET> client_sockets;

    while (true) {
        fd_set read_fds;
        FD_ZERO(&read_fds);
        FD_SET(server_socket, &read_fds);

        for (SOCKET client_sock : client_sockets) {
            FD_SET(client_sock, &read_fds);
        }

        timeval timeout{};
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;

        int activity = select(0, &read_fds, nullptr, nullptr, &timeout);

        if (activity == SOCKET_ERROR) {
            spdlog::error("Select error: {}", WSAGetLastError());
            break;
        }

        // accept new client connections
        if (FD_ISSET(server_socket, &read_fds)) {
            sockaddr_in client_address{};
            int addr_len = sizeof(client_address);
            SOCKET new_client_socket = accept(server_socket, reinterpret_cast<sockaddr*>(&client_address), &addr_len);

            if (new_client_socket != INVALID_SOCKET) {
                client_sockets.push_back(new_client_socket);
                spdlog::info("New client connected. Total clients: {}", client_sockets.size());
            }
        }

        // process data from connected clients
        for (auto it = client_sockets.begin(); it != client_sockets.end();) {
            SOCKET client_sock = *it;

            if (FD_ISSET(client_sock, &read_fds)) {
                char buffer[BUFFER_SIZE] = {0};
                int bytes_read = recv(client_sock, buffer, sizeof(buffer) - 1, 0);

                if (bytes_read <= 0) {
                    spdlog::info("Client disconnected.");
                    closesocket(client_sock);
                    it = client_sockets.erase(it);
                    continue;
                } else {
                    std::string received_str(buffer, bytes_read);
                    spdlog::info("Received data from client: {}", received_str);

                    json payload = {{"event", "broadcast_data"},
                                    {"sender_socket", static_cast<unsigned long long>(client_sock)},
                                    {"payload", received_str},
                                    {"status", "ok"}};

                    std::string message_data = payload.dump() + "\n";

                    spdlog::info("Broadcasting JSON to {} clients...", client_sockets.size());
                    for (SOCKET target_sock : client_sockets) {
                        send(target_sock, message_data.c_str(), static_cast<int>(message_data.length()), 0);
                    }
                }
            }
            ++it;
        }
    }

    for (SOCKET client_sock : client_sockets) {
        closesocket(client_sock);
    }
    closesocket(server_socket);
    WSACleanup();

    return 0;
}