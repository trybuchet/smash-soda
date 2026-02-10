#ifndef STREAM_H
#define STREAM_H

#include <iostream>
#include <WinSock2.h>
#include <Windows.h>
#define ASIO_STANDALONE
#define _WEBSOCKETPP_CPP11_RANDOM_DEVICE_
#define _WEBSOCKETPP_CPP11_TYPE_TRAITS_
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/client.hpp>
#include <nlohmann/json.hpp>
#include <set>
#include <mutex>
#include <thread>
#include <functional> // For std::bind
#include <memory> // For std::shared_ptr
#include <string>

typedef websocketpp::client<websocketpp::config::asio> client;

/// <summary>
/// Handles the hotseat system.
/// </summary>
class StreamHandler {
public:

	class ProviderCreds {
	public:
		std::string username; // Username of the provider
		std::string token; // Access token
		std::string refreshToken; // Refresh token
		int expiresIn; // Expiration time in seconds

		ProviderCreds() : username(""), token(""), refreshToken(""), expiresIn(0) {}
	};

	static StreamHandler instance; // Singleton instance

	ProviderCreds twitch; // Twitch credentials

	// Constructor
	StreamHandler();
	StreamHandler(const StreamHandler&) = delete;
	StreamHandler& operator=(const StreamHandler&) = delete;

	void connectToTwitchChat();
	void disconnect();

private:
	client wsClient_; // WebSocket client
	std::thread wsThread_; // WebSocket thread
	std::set<websocketpp::connection_hdl, std::owner_less<websocketpp::connection_hdl>> clients_; // Set of connected clients
	std::mutex clientsMutex_; // Mutex for thread safety
	bool isRunning_ = false; // Is the WebSocket client running?
	bool _asioInited = false; // Is ASIO initialized?
	void onOpen(websocketpp::connection_hdl hdl);
	void onClose(websocketpp::connection_hdl hdl);
	void onMessage(websocketpp::connection_hdl hdl, client::message_ptr msg);
};

#endif // STREAM_H