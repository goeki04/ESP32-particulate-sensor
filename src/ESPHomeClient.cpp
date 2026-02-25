#include "pch.h"
#include "ESPHomeClient.h"
#include <thread>

ESPHomeClient::ESPHomeClient() : m_IoContext(), m_Resolver(m_IoContext), m_Socket(m_IoContext), m_ReceiveBuffer{0} {}

ESPHomeClient::~ESPHomeClient()
{
	m_IoContext.stop();
	if (m_NetworkThread.joinable()) {
		m_NetworkThread.join();
	}
}

void ESPHomeClient::connect(const std::string& host, const std::string& port) {
    m_Resolver.async_resolve(host, port, [this](auto ec, auto results) {
            if (!ec) {
                handleResolve(results);
            }
        });
    if (!m_NetworkThread.joinable()) {
        m_NetworkThread = std::thread([this]() { m_IoContext.run(); });
    }
}

void ESPHomeClient::handleResolve(asio::ip::tcp::resolver::results_type results) {
    asio::async_connect(m_Socket, results,
        [this](const std::error_code& ec, const asio::ip::tcp::endpoint& endpoint) {
            if (!ec) {
                SDL_Log("Verbunden mit %s", endpoint.address().to_string().c_str());
                startRead();
            }
            else {
                SDL_Log("Connect-Fehler: %s", ec.message().c_str());
            }
        });
}

void ESPHomeClient::startRead()
{
    m_Socket.async_read_some(asio::buffer(m_ReceiveBuffer),
        [this](const std::error_code& ec, std::size_t bytes_transferred) {
            if (!ec) {
                m_Decoder.appendData(m_ReceiveBuffer, bytes_transferred);
                startRead();
            }
            else {
                SDL_Log("Verbindung zum ESP32 verloren: %s", ec.message().c_str());
            }
        });
}
