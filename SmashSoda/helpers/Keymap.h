#pragma once
#include <iostream>
#include <vector>
#include <string>

// Define the custom Map class
class Map {
public:
    std::string keyName;
    int keyCode;

    // Constructor for Map class
    Map(const std::string& keyName, int keyCode)
        : keyName(keyName), keyCode(keyCode) {}
};

class Keymap {
public:
    // Vector of Map objects
    std::vector<Map> keyMap = {
        Map("A", 4),
        Map("B", 5),
        Map("C", 6),
        Map("D", 7),
        Map("E", 8),
        Map("F", 9),
        Map("G", 10),
        Map("H", 11),
        Map("I", 12),
        Map("J", 13),
        Map("K", 14),
        Map("L", 15),
        Map("M", 16),
        Map("N", 17),
        Map("O", 18),
        Map("P", 19),
        Map("Q", 20),
        Map("R", 21),
        Map("S", 22),
        Map("T", 23),
        Map("U", 24),
        Map("V", 25),
        Map("W", 26),
        Map("X", 27),
        Map("Y", 28),
        Map("Z", 29),
        Map("1", 30),
        Map("2", 31),
        Map("3", 32),
        Map("4", 33),
        Map("5", 34),
        Map("6", 35),
        Map("7", 36),
        Map("8", 37),
        Map("9", 38),
        Map("0", 39),
        Map("Enter", 40),
        Map("Escape", 41),
        Map("Backspace", 42),
        Map("Tab", 43),
        Map("Space", 44),
        Map("Minus", 45),
        Map("Equals", 46),
        Map("LBracket", 47),
        Map("RBracket", 48),
        Map("Backslash", 49),
        Map("Semicolon", 51),
        Map("Apostrophe", 52),
        Map("Backtick", 53),
        Map("Comma", 54),
        Map("Period", 55),
        Map("Slash", 56),
        Map("CapsLock", 57),
        Map("F1", 58),
        Map("F2", 59),
        Map("F3", 60),
        Map("F4", 61),
        Map("F5", 62),
        Map("F6", 63),
        Map("F7", 64),
        Map("F8", 65),
        Map("F9", 66),
        Map("F10", 67),
        Map("F11", 68),
        Map("F12", 69),
        Map("PrintScreen", 70),
        Map("ScrollLock", 71),
        Map("Pause", 72),
        Map("Insert", 73),
        Map("Home", 74),
        Map("PageUp", 75),
        Map("Delete", 76),
        Map("End", 77),
        Map("PageDown", 78),
        Map("Right", 79),
        Map("Left", 80),
        Map("Down", 81),
        Map("Up", 82),
        Map("NumLock", 83),
        Map("KP_Divide", 84),
        Map("KP_Multiply", 85),
        Map("KP_Minus", 86),
        Map("KP_Plus", 87),
        Map("KP_Enter", 88),
        Map("KP_1", 89),
        Map("KP_2", 90),
        Map("KP_3", 91),
        Map("KP_4", 92),
        Map("KP_5", 93),
        Map("KP_6", 94),
        Map("KP_7", 95),
        Map("KP_8", 96),
        Map("KP_9", 97),
        Map("KP_0", 98),
        Map("KP_Period", 99),
        Map("Application", 101),
        Map("F13", 104),
        Map("F14", 105),
        Map("F15", 106),
        Map("F16", 107),
        Map("F17", 108),
        Map("F18", 109),
        Map("F19", 110),
        Map("Menu", 118),
        Map("Mute", 127),
        Map("VolumeUp", 128),
        Map("VolumeDown", 129),
        Map("LCTRL", 224),
        Map("LShift", 225),
        Map("LAlt", 226),
        Map("LGUI", 227),
        Map("RCTRL", 228),
        Map("RShift", 229),
        Map("RAlt", 230),
        Map("RGUI", 231),
        Map("AudioNext", 258),
        Map("AudioPrev", 259),
        Map("AudioStop", 260),
        Map("AudioPlay", 261),
        Map("AudioMute", 262),
        Map("MediaSelect", 263),
        Map("None", 0x0000),
        Map("LShift", 0x0001),
        Map("RShift", 0x0002),
        Map("LCtrl", 0x0040),
        Map("RCtrl", 0x0080),
        Map("LAlt", 0x0100),
        Map("RAlt", 0x0200),
        Map("LGUI", 0x0400),
        Map("RGUI", 0x0800),
        Map("NumLock", 0x1000),
        Map("CapsLock", 0x2000),
    };

    // Constructor
    Keymap() {}

    // Function to find key name by code (decimal value)
    std::string findKeyByValue(int value) {
        for (const auto& map : keyMap) {
            if (map.keyCode == value) {
                return map.keyName;
            }
        }
        return "[Unknown Key]";
    }

    // Function to find key code by key name
    int findValueByKey(const std::string& key) {
        for (const auto& map : keyMap) {
            if (map.keyName == key) {
                return map.keyCode;
            }
        }
        return -1;
    }
};


