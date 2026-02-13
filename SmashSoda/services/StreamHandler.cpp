#include "StreamHandler.h"

StreamHandler StreamHandler::instance;

/**
 * Constructor for StreamHandler.
 */
StreamHandler::StreamHandler() : isRunning_(false), _asioInited(false) {
	if (!_asioInited) {
		wsClient_.init_asio();
		_asioInited = true;
	}
}

/**
 * Connect to the Twitch chat IRC.
 * 
 */
void StreamHandler::connectToTwitchChat() {

	// Connect to Twitch IRC websocket
	std::string uri = "wss://irc-ws.chat.twitch.tv:443";
	wsClient_.set_open_handler(std::bind(&StreamHandler::onOpen, this, std::placeholders::_1));
	wsClient_.set_close_handler(std::bind(&StreamHandler::onClose, this, std::placeholders::_1));
	wsClient_.set_message_handler(std::bind(&StreamHandler::onMessage, this, std::placeholders::_1, std::placeholders::_2));
	websocketpp::lib::error_code ec;
	client::connection_ptr con = wsClient_.get_connection(uri, ec);
	if (ec) {
		std::cerr << "Could not create connection because: " << ec.message() << std::endl;
		return;
	}
	wsClient_.connect(con);
	isRunning_ = true;

	// Start the ASIO event loop
	wsClient_.run();

}

/**
 * Disconnects the websocket.
 */
void StreamHandler::disconnect() {
	// Close the connection
	std::lock_guard<std::mutex> lock(clientsMutex_);
	for (auto& client : clients_) {
		wsClient_.close(client, websocketpp::close::status::normal, "Client disconnected");
	}
	clients_.clear();
	isRunning_ = false;
	if (wsClient_.is_listening()) {
		wsClient_.stop_listening();
	}
}

/**
 * Handles the opening of a connection.
 *
 * @param hdl The connection handle.
 */
void StreamHandler::onOpen(websocketpp::connection_hdl hdl) {
	std::lock_guard<std::mutex> lock(clientsMutex_);
	clients_.insert(hdl);
}

/**
 * Handles the closing of a connection.
 *
 * @param hdl The connection handle.
 */
void StreamHandler::onClose(websocketpp::connection_hdl hdl) {
	std::lock_guard<std::mutex> lock(clientsMutex_);
	clients_.erase(hdl);
}

/**
 * Handles incoming messages.
 *
 * @param hdl The connection handle.
 * @param msg The message pointer.
 */
void StreamHandler::onMessage(websocketpp::connection_hdl hdl, client::message_ptr msg) {
}