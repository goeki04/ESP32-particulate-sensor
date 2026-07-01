#pragma once

/**
 * @file a_IWebsocketClient.hpp
 * @brief Interface definition for WebSocket client implementations.
 */

#include <string>
#include <functional>
#include "a_Primitives.hpp"
namespace Andromeda {
	/**
	 * @struct WebsocketReq
	 * @brief Describes the parameters needed to establish a WebSocket connection.
	 */
	struct WebsocketReq {
		i32 port;                          ///< Target port number of the WebSocket server.
		std::string host;                ///< Host or IP address of the WebSocket server.
		std::string path;                  ///< Request path/endpoint used for the WebSocket handshake.
		std::vector<std::string> headers;  ///< Additional HTTP headers to send during the handshake.
	};

	/**
	 * @class IWebsocketClient
	 * @brief Abstract interface for a WebSocket client.
	 *
	 * Defines the minimal contract a WebSocket client implementation must fulfill:
	 * connecting to and disconnecting from a server, sending payloads, and
	 * notifying callers of incoming messages via a callback.
	 */
	class IWebsocketClient {
	public:
		/** @brief Virtual destructor to allow safe destruction through the interface pointer. */
		virtual ~IWebsocketClient() = default;

		/** @brief Establishes the WebSocket connection to the configured server. */
		virtual void connect() = 0;

		/** @brief Closes the active WebSocket connection. */
		virtual void disconnect() = 0;

		/**
		 * @brief Sends a text payload over the WebSocket connection.
		 * @param payload The message content to transmit.
		 */
		virtual void send(const std::string& payload) = 0;

		/**
		 * @brief Registers a callback invoked whenever a message is received.
		 * @param callback Function called with the received message content.
		 */
		virtual void onMessageReceived(std::function<void(const std::string&)> callback) = 0;
	};
}