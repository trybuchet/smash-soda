#include "InputControlService.h"

namespace {
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

    return sendKeyboardInput(translation.vk, flags);
}

bool InputControlService::handleParsecMouseButton(const ParsecMouseButtonMessage& mouseButton) const {
    MouseButton button;
    if (!parsecMouseToService(mouseButton.button, button)) {
        return false;
    }

    return mouseButton.pressed ? mouseButtonDown(button) : mouseButtonUp(button);
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

    return setMousePosition(mouseMotion.x, mouseMotion.y);
}
