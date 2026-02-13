#include "InputControlService.h"
#include "../core/Config.h"

#include <algorithm>
#include <cmath>
#include <vector>

namespace {
    constexpr ULONGLONG HOST_OVERRIDE_WINDOW_MS = 300;

    BOOL CALLBACK collectMonitorRects(HMONITOR, HDC, LPRECT rect, LPARAM data) {
        if (data == 0 || rect == nullptr) {
            return TRUE;
        }

        auto* monitorRects = reinterpret_cast<std::vector<RECT>*>(data);
        monitorRects->push_back(*rect);
        return TRUE;
    }

    bool getSelectedMonitorRect(RECT& outRect) {
        std::vector<RECT> monitorRects;
        EnumDisplayMonitors(nullptr, nullptr, collectMonitorRects, reinterpret_cast<LPARAM>(&monitorRects));
        if (monitorRects.empty()) {
            return false;
        }

        size_t monitorIndex = Config::cfg.video.monitor;
        if (monitorIndex >= monitorRects.size()) {
            monitorIndex = 0;
        }

        outRect = monitorRects[monitorIndex];
        return true;
    }

    void getStreamMouseResolution(const RECT& monitorRect, int& width, int& height) {
        width = monitorRect.right - monitorRect.left;
        height = monitorRect.bottom - monitorRect.top;

        const size_t resolutionIndex = Config::cfg.video.resolutionIndex;
        if (resolutionIndex > 0 && resolutionIndex < Config::cfg.resolutions.size()) {
            const int configuredWidth = Config::cfg.resolutions[resolutionIndex].width;
            const int configuredHeight = Config::cfg.resolutions[resolutionIndex].height;
            if (configuredWidth > 0 && configuredHeight > 0) {
                width = configuredWidth;
                height = configuredHeight;
            }
        }
    }

    LONG remapAxis(LONG value, int sourceSize, int destinationSize, LONG destinationOrigin) {
        if (sourceSize <= 1 || destinationSize <= 1) {
            return destinationOrigin;
        }

        const LONG sourceMax = static_cast<LONG>(sourceSize - 1);
        const LONG clamped = std::clamp(value, 0L, sourceMax);
        const double normalized = static_cast<double>(clamped) / static_cast<double>(sourceMax);
        const LONG mapped = static_cast<LONG>(std::lround(normalized * static_cast<double>(destinationSize - 1)));
        return destinationOrigin + mapped;
    }

    void remapAbsoluteMouseToDesktop(LONG inX, LONG inY, LONG& outX, LONG& outY) {
        RECT monitorRect = {};
        if (!getSelectedMonitorRect(monitorRect)) {
            outX = inX;
            outY = inY;
            return;
        }

        int inputWidth = 0;
        int inputHeight = 0;
        getStreamMouseResolution(monitorRect, inputWidth, inputHeight);

        const int monitorWidth = monitorRect.right - monitorRect.left;
        const int monitorHeight = monitorRect.bottom - monitorRect.top;

        outX = remapAxis(inX, inputWidth, monitorWidth, monitorRect.left);
        outY = remapAxis(inY, inputHeight, monitorHeight, monitorRect.top);
    }

    struct KeyTranslation {
        WORD vk;
        bool extended;
    };

    bool parsecKeyToWin(ParsecKeycode code, KeyTranslation& out) {
        out = { 0, false };

        if (code >= KEY_A && code <= KEY_Z) {
            out.vk = static_cast<WORD>('A' + (code - KEY_A));
            return true;
        }

        if (code >= KEY_1 && code <= KEY_9) {
            out.vk = static_cast<WORD>('1' + (code - KEY_1));
            return true;
        }

        if (code == KEY_0) {
            out.vk = static_cast<WORD>('0');
            return true;
        }

        if (code >= KEY_F1 && code <= KEY_F12) {
            out.vk = static_cast<WORD>(VK_F1 + (code - KEY_F1));
            return true;
        }

        if (code >= KEY_F13 && code <= KEY_F19) {
            out.vk = static_cast<WORD>(VK_F13 + (code - KEY_F13));
            return true;
        }

        if (code >= KEY_KP_1 && code <= KEY_KP_9) {
            out.vk = static_cast<WORD>(VK_NUMPAD1 + (code - KEY_KP_1));
            return true;
        }

        switch (code) {
        case KEY_ENTER: out.vk = VK_RETURN; return true;
        case KEY_ESCAPE: out.vk = VK_ESCAPE; return true;
        case KEY_BACKSPACE: out.vk = VK_BACK; return true;
        case KEY_TAB: out.vk = VK_TAB; return true;
        case KEY_SPACE: out.vk = VK_SPACE; return true;
        case KEY_MINUS: out.vk = VK_OEM_MINUS; return true;
        case KEY_EQUALS: out.vk = VK_OEM_PLUS; return true;
        case KEY_LBRACKET: out.vk = VK_OEM_4; return true;
        case KEY_RBRACKET: out.vk = VK_OEM_6; return true;
        case KEY_BACKSLASH: out.vk = VK_OEM_5; return true;
        case KEY_SEMICOLON: out.vk = VK_OEM_1; return true;
        case KEY_APOSTROPHE: out.vk = VK_OEM_7; return true;
        case KEY_BACKTICK: out.vk = VK_OEM_3; return true;
        case KEY_COMMA: out.vk = VK_OEM_COMMA; return true;
        case KEY_PERIOD: out.vk = VK_OEM_PERIOD; return true;
        case KEY_SLASH: out.vk = VK_OEM_2; return true;
        case KEY_CAPSLOCK: out.vk = VK_CAPITAL; return true;
        case KEY_PRINTSCREEN: out.vk = VK_SNAPSHOT; out.extended = true; return true;
        case KEY_SCROLLLOCK: out.vk = VK_SCROLL; return true;
        case KEY_PAUSE: out.vk = VK_PAUSE; return true;
        case KEY_INSERT: out.vk = VK_INSERT; out.extended = true; return true;
        case KEY_HOME: out.vk = VK_HOME; out.extended = true; return true;
        case KEY_PAGEUP: out.vk = VK_PRIOR; out.extended = true; return true;
        case KEY_DELETE: out.vk = VK_DELETE; out.extended = true; return true;
        case KEY_END: out.vk = VK_END; out.extended = true; return true;
        case KEY_PAGEDOWN: out.vk = VK_NEXT; out.extended = true; return true;
        case KEY_RIGHT: out.vk = VK_RIGHT; out.extended = true; return true;
        case KEY_LEFT: out.vk = VK_LEFT; out.extended = true; return true;
        case KEY_DOWN: out.vk = VK_DOWN; out.extended = true; return true;
        case KEY_UP: out.vk = VK_UP; out.extended = true; return true;
        case KEY_NUMLOCK: out.vk = VK_NUMLOCK; out.extended = true; return true;
        case KEY_KP_DIVIDE: out.vk = VK_DIVIDE; out.extended = true; return true;
        case KEY_KP_MULTIPLY: out.vk = VK_MULTIPLY; return true;
        case KEY_KP_MINUS: out.vk = VK_SUBTRACT; return true;
        case KEY_KP_PLUS: out.vk = VK_ADD; return true;
        case KEY_KP_ENTER: out.vk = VK_RETURN; out.extended = true; return true;
        case KEY_KP_0: out.vk = VK_NUMPAD0; return true;
        case KEY_KP_PERIOD: out.vk = VK_DECIMAL; return true;
        case KEY_APPLICATION:
        case KEY_MENU: out.vk = VK_APPS; return true;
        case KEY_MUTE:
        case KEY_AUDIOMUTE: out.vk = VK_VOLUME_MUTE; return true;
        case KEY_VOLUMEUP: out.vk = VK_VOLUME_UP; return true;
        case KEY_VOLUMEDOWN: out.vk = VK_VOLUME_DOWN; return true;
        case KEY_LCTRL: out.vk = VK_LCONTROL; return true;
        case KEY_LSHIFT: out.vk = VK_LSHIFT; return true;
        case KEY_LALT: out.vk = VK_LMENU; return true;
        case KEY_LGUI: out.vk = VK_LWIN; out.extended = true; return true;
        case KEY_RCTRL: out.vk = VK_RCONTROL; out.extended = true; return true;
        case KEY_RSHIFT: out.vk = VK_RSHIFT; return true;
        case KEY_RALT: out.vk = VK_RMENU; out.extended = true; return true;
        case KEY_RGUI: out.vk = VK_RWIN; out.extended = true; return true;
        case KEY_AUDIONEXT: out.vk = VK_MEDIA_NEXT_TRACK; return true;
        case KEY_AUDIOPREV: out.vk = VK_MEDIA_PREV_TRACK; return true;
        case KEY_AUDIOSTOP: out.vk = VK_MEDIA_STOP; return true;
        case KEY_AUDIOPLAY: out.vk = VK_MEDIA_PLAY_PAUSE; return true;
        case KEY_MEDIASELECT: out.vk = VK_LAUNCH_MEDIA_SELECT; return true;
        default:
            return false;
        }
    }

    bool parsecMouseToService(ParsecMouseButton button, InputControlService::MouseButton& out) {
        switch (button) {
        case MOUSE_L:
            out = InputControlService::MouseButton::Left;
            return true;
        case MOUSE_R:
            out = InputControlService::MouseButton::Right;
            return true;
        case MOUSE_MIDDLE:
            out = InputControlService::MouseButton::Middle;
            return true;
        case MOUSE_X1:
            out = InputControlService::MouseButton::X1;
            return true;
        case MOUSE_X2:
            out = InputControlService::MouseButton::X2;
            return true;
        default:
            return false;
        }
    }

    DWORD mouseDownFlag(InputControlService::MouseButton button) {
        switch (button) {
        case InputControlService::MouseButton::Left:
            return MOUSEEVENTF_LEFTDOWN;
        case InputControlService::MouseButton::Right:
            return MOUSEEVENTF_RIGHTDOWN;
        case InputControlService::MouseButton::Middle:
            return MOUSEEVENTF_MIDDLEDOWN;
        case InputControlService::MouseButton::X1:
        case InputControlService::MouseButton::X2:
            return MOUSEEVENTF_XDOWN;
        default:
            return 0;
        }
    }

    DWORD mouseUpFlag(InputControlService::MouseButton button) {
        switch (button) {
        case InputControlService::MouseButton::Left:
            return MOUSEEVENTF_LEFTUP;
        case InputControlService::MouseButton::Right:
            return MOUSEEVENTF_RIGHTUP;
        case InputControlService::MouseButton::Middle:
            return MOUSEEVENTF_MIDDLEUP;
        case InputControlService::MouseButton::X1:
        case InputControlService::MouseButton::X2:
            return MOUSEEVENTF_XUP;
        default:
            return 0;
        }
    }

    DWORD mouseDataForButton(InputControlService::MouseButton button) {
        switch (button) {
        case InputControlService::MouseButton::X1:
            return XBUTTON1;
        case InputControlService::MouseButton::X2:
            return XBUTTON2;
        default:
            return 0;
        }
    }
}

InputControlService& InputControlService::instance() {
    static InputControlService s_instance;
    return s_instance;
}

void InputControlService::noteHostPhysicalInput() {
    _lastHostPhysicalInputMs.store(GetTickCount64(), std::memory_order_relaxed);
}

bool InputControlService::isHostOverrideActive() const {
    const ULONGLONG lastHostInput = _lastHostPhysicalInputMs.load(std::memory_order_relaxed);
    if (lastHostInput == 0) {
        return false;
    }

    const ULONGLONG now = GetTickCount64();
    return now >= lastHostInput && (now - lastHostInput) <= HOST_OVERRIDE_WINDOW_MS;
}

bool InputControlService::shouldBlockGuestMessage(const ParsecMessage& message) const {
    if (message.type != MESSAGE_KEYBOARD &&
        message.type != MESSAGE_MOUSE_BUTTON &&
        message.type != MESSAGE_MOUSE_WHEEL &&
        message.type != MESSAGE_MOUSE_MOTION) {
        return false;
    }

    return isHostOverrideActive();
}

void InputControlService::releaseGuestHeldInputs() const {
    std::lock_guard<std::mutex> lock(_stateMutex);

    for (size_t i = 0; i < _guestKeysDown.size(); ++i) {
        if (_guestKeysDown[i]) {
            sendKeyboardInput(static_cast<WORD>(i), KEYEVENTF_KEYUP);
            _guestKeysDown[i] = false;
        }
    }

    if (_guestLeftDown) {
        sendMouseInput(MOUSEEVENTF_LEFTUP);
        _guestLeftDown = false;
    }
    if (_guestRightDown) {
        sendMouseInput(MOUSEEVENTF_RIGHTUP);
        _guestRightDown = false;
    }
    if (_guestMiddleDown) {
        sendMouseInput(MOUSEEVENTF_MIDDLEUP);
        _guestMiddleDown = false;
    }
    if (_guestX1Down) {
        sendMouseInput(MOUSEEVENTF_XUP, XBUTTON1);
        _guestX1Down = false;
    }
    if (_guestX2Down) {
        sendMouseInput(MOUSEEVENTF_XUP, XBUTTON2);
        _guestX2Down = false;
    }
}

bool InputControlService::setMousePosition(LONG x, LONG y) const {
    return SetCursorPos(x, y) == TRUE;
}

bool InputControlService::mouseButtonDown(MouseButton button) const {
    const DWORD flags = mouseDownFlag(button);
    if (flags == 0) {
        return false;
    }

    return sendMouseInput(flags, mouseDataForButton(button));
}

bool InputControlService::mouseButtonUp(MouseButton button) const {
    const DWORD flags = mouseUpFlag(button);
    if (flags == 0) {
        return false;
    }

    return sendMouseInput(flags, mouseDataForButton(button));
}

bool InputControlService::mouseClick(MouseButton button) const {
    return mouseButtonDown(button) && mouseButtonUp(button);
}

bool InputControlService::keyDown(WORD virtualKey) const {
    return sendKeyboardInput(virtualKey, 0);
}

bool InputControlService::keyUp(WORD virtualKey) const {
    return sendKeyboardInput(virtualKey, KEYEVENTF_KEYUP);
}

bool InputControlService::keyPress(WORD virtualKey) const {
    return keyDown(virtualKey) && keyUp(virtualKey);
}

bool InputControlService::handleParsecMessage(const ParsecMessage& message) const {
    const bool blockGuestMessage = shouldBlockGuestMessage(message);
    if (blockGuestMessage) {
        bool shouldRelease = false;
        {
            std::lock_guard<std::mutex> lock(_stateMutex);
            if (!_wasBlockingGuestInput) {
                _wasBlockingGuestInput = true;
                shouldRelease = true;
            }
        }

        if (shouldRelease) {
            releaseGuestHeldInputs();
        }
        return true;
    }

    {
        std::lock_guard<std::mutex> lock(_stateMutex);
        _wasBlockingGuestInput = false;
    }

    switch (message.type) {
    case MESSAGE_KEYBOARD:
        return handleParsecKeyboard(message.keyboard);
    case MESSAGE_MOUSE_BUTTON:
        return handleParsecMouseButton(message.mouseButton);
    case MESSAGE_MOUSE_WHEEL:
        return handleParsecMouseWheel(message.mouseWheel);
    case MESSAGE_MOUSE_MOTION:
        return handleParsecMouseMotion(message.mouseMotion);
    default:
        return false;
    }
}

bool InputControlService::sendMouseInput(DWORD flags, DWORD mouseData) const {
    INPUT input = {};
    input.type = INPUT_MOUSE;
    input.mi.dwFlags = flags;
    input.mi.mouseData = mouseData;

    return SendInput(1, &input, sizeof(INPUT)) == 1;
}

bool InputControlService::sendKeyboardInput(WORD virtualKey, DWORD flags) const {
    INPUT input = {};
    input.type = INPUT_KEYBOARD;
    input.ki.wVk = virtualKey;
    input.ki.dwFlags = flags;

    return SendInput(1, &input, sizeof(INPUT)) == 1;
}

bool InputControlService::handleParsecKeyboard(const ParsecKeyboardMessage& keyboard) const {
    KeyTranslation translation;
    if (!parsecKeyToWin(keyboard.code, translation)) {
        return false;
    }

    DWORD flags = keyboard.pressed ? 0 : KEYEVENTF_KEYUP;
    if (translation.extended) {
        flags |= KEYEVENTF_EXTENDEDKEY;
    }

    const bool ok = sendKeyboardInput(translation.vk, flags);
    if (ok) {
        std::lock_guard<std::mutex> lock(_stateMutex);
        _guestKeysDown[translation.vk] = keyboard.pressed;
    }
    return ok;
}

bool InputControlService::handleParsecMouseButton(const ParsecMouseButtonMessage& mouseButton) const {
    MouseButton button;
    if (!parsecMouseToService(mouseButton.button, button)) {
        return false;
    }

    const bool ok = mouseButton.pressed ? mouseButtonDown(button) : mouseButtonUp(button);
    if (ok) {
        std::lock_guard<std::mutex> lock(_stateMutex);
        switch (button) {
        case MouseButton::Left:
            _guestLeftDown = mouseButton.pressed;
            break;
        case MouseButton::Right:
            _guestRightDown = mouseButton.pressed;
            break;
        case MouseButton::Middle:
            _guestMiddleDown = mouseButton.pressed;
            break;
        case MouseButton::X1:
            _guestX1Down = mouseButton.pressed;
            break;
        case MouseButton::X2:
            _guestX2Down = mouseButton.pressed;
            break;
        default:
            break;
        }
    }

    return ok;
}

bool InputControlService::handleParsecMouseWheel(const ParsecMouseWheelMessage& mouseWheel) const {
    bool ok = true;

    if (mouseWheel.x != 0) {
        const int horizontalDelta = mouseWheel.x * WHEEL_DELTA;
        ok = ok && sendMouseInput(MOUSEEVENTF_HWHEEL, static_cast<DWORD>(horizontalDelta));
    }

    if (mouseWheel.y != 0) {
        // Parsec wheel y is negative for up. Win32 positive wheel data scrolls up.
        const int verticalDelta = -mouseWheel.y * WHEEL_DELTA;
        ok = ok && sendMouseInput(MOUSEEVENTF_WHEEL, static_cast<DWORD>(verticalDelta));
    }

    return ok;
}

bool InputControlService::handleParsecMouseMotion(const ParsecMouseMotionMessage& mouseMotion) const {
    if (mouseMotion.relative) {
        INPUT input = {};
        input.type = INPUT_MOUSE;
        input.mi.dwFlags = MOUSEEVENTF_MOVE;
        input.mi.dx = mouseMotion.x;
        input.mi.dy = mouseMotion.y;
        return SendInput(1, &input, sizeof(INPUT)) == 1;
    }

    LONG absoluteX = 0;
    LONG absoluteY = 0;
    remapAbsoluteMouseToDesktop(mouseMotion.x, mouseMotion.y, absoluteX, absoluteY);
    return setMousePosition(absoluteX, absoluteY);
}
