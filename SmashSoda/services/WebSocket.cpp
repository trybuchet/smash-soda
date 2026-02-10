#include "../Hosting.h"
extern Hosting g_hosting;
#include "WebSocket.h"
#include "OverlayService.h"

WebSocket WebSocket::instance;

WebSocket::WebSocket() {
    // Constructor implementation (if needed)
}

void WebSocket::createServer(uint16_t port) {
    if (!_asioInited) {
        server_.init_asio();
        _asioInited = true;
    }
    else
    {
        server_.reset();
    }
    server_.set_open_handler(std::bind(&WebSocket::onOpen, this, std::placeholders::_1));
    server_.set_close_handler(std::bind(&WebSocket::onClose, this, std::placeholders::_1));
    server_.set_message_handler(std::bind(&WebSocket::onMessage, this, std::placeholders::_1, std::placeholders::_2));
    
    serverThread_ = std::thread([this, port]() {
        try {
            server_.listen(port);
            server_.start_accept();
            isRunning_ = true;
            g_hosting.logMessage("WebSocket server started.");

            // Start overlay
            if (Config::cfg.overlay.enabled) {
                OverlayService::instance().start();
            }

            server_.run();
        } catch (const std::exception& e) {
            g_hosting.logMessage("Server error: " + std::string(e.what()));
        }
        isRunning_ = false;
        g_hosting.logMessage("WebSocket server stopped.");
    });
}

void WebSocket::stopServer() {
    server_.stop_listening();
    server_.stop();
    if (serverThread_.joinable()) {
        serverThread_.join();
    }
    if (Config::cfg.overlay.enabled) {
        OverlayService::instance().stop();
    }
    isRunning_ = false;
}

void WebSocket::sendMessageToAll(const std::string& message) {
    //g_hosting.logMessage("Sending WebSocket message to all clients.");

    // If not running, return
    if (!isRunning_) {
        return;
    }

    // If no clients are connected, return
    if (clients_.empty()) {
        return;
    }

    //std::lock_guard<std::mutex> lock(clientsMutex_);
    for (auto& client : clients_) {
        server_.send(client, message, websocketpp::frame::opcode::text);
    }
}

bool WebSocket::isRunning() {
    return isRunning_;
}

void WebSocket::onOpen(websocketpp::connection_hdl hdl) {
    std::lock_guard<std::mutex> lock(clientsMutex_);
    clients_.insert(hdl);
    g_hosting.logMessage("WebSocket client connected.");
    OverlayService::instance().onWebSocketOpen();
}

void WebSocket::onClose(websocketpp::connection_hdl hdl) {
    std::lock_guard<std::mutex> lock(clientsMutex_);
    clients_.erase(hdl);
    OverlayService::instance().onWebSocketClose();
}

void WebSocket::onMessage(websocketpp::connection_hdl hdl, server::message_ptr msg) {
    json j = json::parse(msg->get_payload());
    if (j["event"] == "chat:send") {
        string message;
        j.at("data").get_to(message);

        g_hosting.sendHostMessage(message.c_str());
    } 
    // else if (j["event"] == "chat:external") {
    //     string source;
    //     string message;
    //     string user;

    //     j.at("data").at("user").at("display-name").get_to(user);
    //     j.at("data").at("message").get_to(message);
    //     try {
	// 		j.at("data").at("source").get_to(source);
    //     }
    //     catch (json::out_of_range& e) {
	// 		source = "Twitch";
	// 	}

    //     g_hosting.messageFromExternalSource(source, user, message);
	// }
	// else if (j["event"] == "auth:twitch") {
	// 	//StreamHandler::instance.twitch.token = j["data"]["token"];
	// 	//StreamHandler::instance.twitch.refreshToken = j["data"]["refreshToken"];
	// 	//StreamHandler::instance.twitch.expiresIn = j["data"]["expiresIn"];
	// }
}


