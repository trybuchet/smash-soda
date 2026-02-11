#pragma once

#include <Windows.h>
#include "parsec.h"

class InputControlService {
public:
    enum class MouseButton {
        Left,
        Right,
        Middle,
        X1,
        X2
    };

    static InputControlService& instance();

    bool setMousePosition(LONG x, LONG y) const;

    bool mouseButtonDown(MouseButton button) const;
    bool mouseButtonUp(MouseButton button) const;
    bool mouseClick(MouseButton button) const;

    bool keyDown(WORD virtualKey) const;
    bool keyUp(WORD virtualKey) const;
    bool keyPress(WORD virtualKey) const;

    bool handleParsecMessage(const ParsecMessage& message) const;

private:
    InputControlService() = default;

    bool sendMouseInput(DWORD flags, DWORD mouseData = 0) const;
    bool sendKeyboardInput(WORD virtualKey, DWORD flags) const;
    bool handleParsecKeyboard(const ParsecKeyboardMessage& keyboard) const;
    bool handleParsecMouseButton(const ParsecMouseButtonMessage& mouseButton) const;
    bool handleParsecMouseWheel(const ParsecMouseWheelMessage& mouseWheel) const;
    bool handleParsecMouseMotion(const ParsecMouseMotionMessage& mouseMotion) const;
};
