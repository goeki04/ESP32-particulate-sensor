#pragma once
#ifdef _WIN32
#define _WIN32_WINNT 0x0A00
#endif
#define ASIO_STANDALONE
#include <asio.hpp>
#include <asio/ts/buffer.hpp>
#include <asio/ts/internet.hpp>
#include "EspHomeDecoder.h"
namespace Andromeda::Network {
	class ESPHomeClient {
	public:
		ESPHomeClient();
		~ESPHomeClient();

		void connect(const std::string& host, const std::string& port);

		ESPHomeDecoder& getDecoder() { return m_Decoder; }
	private:
		void handleResolve(asio::ip::tcp::resolver::results_type results);
		void startRead();
		asio::io_context m_IoContext;
		asio::ip::tcp::resolver m_Resolver;
		asio::ip::tcp::socket m_Socket;
		uint8_t m_ReceiveBuffer[1024];
		std::thread m_NetworkThread;
		ESPHomeDecoder m_Decoder;
	};
}