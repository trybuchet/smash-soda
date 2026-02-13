#pragma once

#include <Windows.h>
#include <string>

enum class OverlayState {
    Stopped,
    Launching,
    RunningNoWS,
    RunningConnected
};

class OverlayService {
public:
    static OverlayService& instance();

    void start();
    void stop();

    void onWebSocketOpen();
    void onWebSocketClose();
    void openMenu();

    OverlayState state() const;
    bool isRunning();
    bool isConnected() const;

private:
    OverlayService() = default;
    std::wstring buildOverlayPath() const;
    bool launchProcess();
    void killProcess();
    bool processIsAlive() const;
    void refreshProcessState();

    OverlayState _state = OverlayState::Stopped;
    DWORD _pid = 0;
    bool _connected = false;
    bool _pendingOpenMenu = false;
};
