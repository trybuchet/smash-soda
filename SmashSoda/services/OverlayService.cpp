#include "../Hosting.h"
extern Hosting g_hosting;

#include "OverlayService.h"
#include "WebSocket.h"
#include <shlwapi.h>

OverlayService& OverlayService::instance() {
    static OverlayService s_instance;
    return s_instance;
}

OverlayState OverlayService::state() const {
    return _state;
}

bool OverlayService::isRunning() const {
    return _state == OverlayState::RunningNoWS || _state == OverlayState::RunningConnected;
}

bool OverlayService::isConnected() const {
    return _connected;
}

void OverlayService::start() {
    if (isRunning()) {
        return;
    }

    _state = OverlayState::Launching;
    _connected = false;

    if (!launchProcess()) {
        _state = OverlayState::Stopped;
        return;
    }

    _state = OverlayState::RunningNoWS;
}

void OverlayService::stop() {
    _connected = false;
    if (_state == OverlayState::Stopped) {
        return;
    }

    killProcess();
    _state = OverlayState::Stopped;
}

void OverlayService::onWebSocketOpen() {
    _connected = true;
    if (_state == OverlayState::RunningNoWS) {
        _state = OverlayState::RunningConnected;
    }
    if (_pendingOpenMenu) {
        _pendingOpenMenu = false;
        WebSocket::instance.sendMessageToAll("{\"event\":\"open:menu\"}");
    }
}

void OverlayService::onWebSocketClose() {
    _connected = false;
    if (_state == OverlayState::RunningConnected) {
        _state = OverlayState::RunningNoWS;
    }
}

void OverlayService::openMenu() {
    if (_connected) {
        WebSocket::instance.sendMessageToAll("{\"event\":\"open:menu\"}");
    } else {
        _pendingOpenMenu = true;
    }
}

std::wstring OverlayService::buildOverlayPath() const {
    wchar_t exePath[MAX_PATH];
    GetModuleFileNameW(NULL, exePath, MAX_PATH);

    PathRemoveFileSpecW(exePath);

    wchar_t overlayPath[MAX_PATH];
    swprintf_s(overlayPath, MAX_PATH, L"%s\\overlay\\SmashGlass.exe", exePath);
    return std::wstring(overlayPath);
}

bool OverlayService::launchProcess() {
    std::wstring path = buildOverlayPath();

    STARTUPINFOW si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    if (!CreateProcessW(
            path.c_str(),
            NULL,
            NULL,
            NULL,
            FALSE,
            0,
            NULL,
            NULL,
            &si,
            &pi)) {
        g_hosting.logMessage("Overlay failed to start. Could not launch SmashGlass.exe.");
        return false;
    }

    _pid = pi.dwProcessId;
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);
    return true;
}

void OverlayService::killProcess() {
    if (_pid == 0) {
        return;
    }
    HANDLE hProcess = OpenProcess(PROCESS_TERMINATE | PROCESS_QUERY_INFORMATION, FALSE, _pid);
    if (hProcess != NULL) {
        TerminateProcess(hProcess, 0);
        CloseHandle(hProcess);
    }
    _pid = 0;
}

bool OverlayService::processIsAlive() const {
    if (_pid == 0) {
        return false;
    }
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, _pid);
    if (hProcess == NULL) {
        return false;
    }
    DWORD exitCode = 0;
    BOOL ok = GetExitCodeProcess(hProcess, &exitCode);
    CloseHandle(hProcess);
    return ok && exitCode == STILL_ACTIVE;
}
