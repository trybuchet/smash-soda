#ifndef WEBSOCKET_H
#define WEBSOCKET_H

#include <iostream>
#include <WinSock2.h>
#include <Windows.h>
#define ASIO_STANDALONE
#define _WEBSOCKETPP_CPP11_RANDOM_DEVICE_
#define _WEBSOCKETPP_CPP11_TYPE_TRAITS_
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <nlohmann/json.hpp>
#include <set>
#include <mutex>
#include <thread>
#include <functional> // For std::bind
#include <memory> // For std::shared_ptr
#include "StreamHandler.h"

typedef websocketpp::server<websocketpp::config::asio> server;

class WebSocket {
public:
    WebSocket();

    void createServer(uint16_t port);
    void stopServer();
    void sendMessageToAll(const std::string& message);
    bool isRunning();

    static WebSocket instance;

private:
    void onOpen(websocketpp::connection_hdl hdl);
    void onClose(websocketpp::connection_hdl hdl);
    void onMessage(websocketpp::connection_hdl hdl, server::message_ptr msg);

    server server_;
    std::thread serverThread_;
    std::set<websocketpp::connection_hdl, std::owner_less<websocketpp::connection_hdl>> clients_;
    std::mutex clientsMutex_;
    bool isRunning_ = false;
    DWORD overlayPID = 0;
    bool _asioInited = false;
};

#endif // WEBSOCKET_H


