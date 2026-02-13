#pragma once

#include "KeyboardMapsUtil.h"
#include "helpers/WStringConvert.h"

//CodeSomnia : ±âº» ÆÄÀÏÀúÀå °æ·Î È®ÀÎ¹× °æ·Î¸¦ °¡Á®¿À±â À§ÇÑ ÇÔ¼ö.


string KeyboardMapsUtil::getKeyMapSavePath() {
    TCHAR tAppdata[1024];
    if (SUCCEEDED(SHGetFolderPath(nullptr, CSIDL_APPDATA, NULL, 0, tAppdata)))
    {
        wstring wAppdata(tAppdata);
        string appdata = WStringToString(wAppdata);
        string dirPath = appdata + "\\SmashSodaTwo\\";

        bool isDirOk = false;

        if (!MTY_FileExists(dirPath.c_str()))
        {
            if (MTY_Mkdir(dirPath.c_str()))
            {
                isDirOk = true;
            }
        }
        else
        {
            isDirOk = true;
        }

        if (isDirOk)
        {
            return dirPath;
        }
    }

    return string();
}

//CodeSomnia : °¢ °ÔÀÓÅ°ÆÐµå°ª°ú ¸ÅÄªµÉ ¼ö ÀÖ´Â ¹®ÀÚ¿­ ¸®ÅÏ
string KeyboardMapsUtil::getGamePadKeyTypeToString(GamePadButtonType_e keyType)
{
    switch (keyType)
    {
    case GamePadButtonType_e::None:
        return "None";
    case GamePadButtonType_e::LLeft:
        return"LLeft";
    case GamePadButtonType_e::LRight:
        return"LRight";
    case GamePadButtonType_e::LUp:
        return"LUp";
    case GamePadButtonType_e::LDown:
        return"LDown";
    case GamePadButtonType_e::RLeft:
        return"RLeft";
    case GamePadButtonType_e::RRight:
        return"RRight";
    case GamePadButtonType_e::RUp:
        return"RUp";
    case GamePadButtonType_e::RDown:
        return"RDown";
    case GamePadButtonType_e::DLeft:
        return"DLeft";
    case GamePadButtonType_e::DRight:
        return"DRight";
    case GamePadButtonType_e::DUp:
        return"DUp";
    case GamePadButtonType_e::DDown:
        return"DDown";
    case GamePadButtonType_e::A:
        return"A";
    case GamePadButtonType_e::B:
        return"B";
    case GamePadButtonType_e::X:
        return"X";
    case GamePadButtonType_e::Y:
        return"Y";
    case GamePadButtonType_e::LB:
        return"LB";
    case GamePadButtonType_e::RB:
        return"RB";
    case GamePadButtonType_e::LT:
        return"LT";
    case GamePadButtonType_e::RT:
        return"RT";
    case GamePadButtonType_e::Back:
        return"Back";
    case GamePadButtonType_e::Start:
        return"Start";
    case GamePadButtonType_e::LThumb:
        return"LThumb";
    case GamePadButtonType_e::RThumb:
        return"RThumb";
    default:
        return "Unknown Game Key";
    }
}


//CodeSomnia : °¢ Å°º¸µå Å°°ª°ú ¸ÅÄªµÉ ¼ö ÀÖ´Â ¹®ÀÚ¿­ ¸®ÅÏ
string KeyboardMapsUtil::getKeyToString(uint16_t keyVal)
{

    switch (keyVal)
    {
    case KEY_A:
        return "A";
    case KEY_B:
        return "B";
    case KEY_C:
        return "C";
    case KEY_D:
        return "D";
    case KEY_E:
        return "E";
    case KEY_F:
        return "F";
    case KEY_G:
        return "G";
    case KEY_H:
        return "H";
    case KEY_I:
        return "I";
    case KEY_J:
        return "J";
    case KEY_K:
        return "K";
    case KEY_L:
        return "L";
    case KEY_M:
        return "M";
    case KEY_N:
        return "N";
    case KEY_O:
        return "O";
    case KEY_P:
        return "P";
    case KEY_Q:
        return "Q";
    case KEY_R:
        return "R";
    case KEY_S:
        return "S";
    case KEY_T:
        return "T";
    case KEY_U:
        return "U";
    case KEY_V:
        return "V";
    case KEY_W:
        return "W";
    case KEY_X:
        return "X";
    case KEY_Y:
        return "Y";
    case KEY_Z:
        return "Z";
    case KEY_1:
        return "1";
    case KEY_2:
        return "2";
    case KEY_3:
        return "3";
    case KEY_4:
        return "4";
    case KEY_5:
        return "5";
    case KEY_6:
        return "6";
    case KEY_7:
        return "7";
    case KEY_8:
        return "8";
    case KEY_9:
        return "9";
    case KEY_0:
        return "0";
    case KEY_ENTER:
        return "Enter";
    case KEY_ESCAPE:
        return "Esc";
    case KEY_BACKSPACE:
        return "Back Space";
    case KEY_TAB:
        return "Tab";
    case KEY_SPACE:
        return "Space";
    case KEY_MINUS:
        return "-";
    case KEY_EQUALS:
        return "=";
    case KEY_LBRACKET:
        return "[";
    case KEY_RBRACKET:
        return "]";
    case KEY_BACKSLASH:
        return "\\";
    case KEY_SEMICOLON:
        return ";";
    case KEY_APOSTROPHE:
        return "'";
    case KEY_BACKTICK:
        return "`";
    case KEY_COMMA:
        return ",";
    case KEY_PERIOD:
        return ".";
    case KEY_SLASH:
        return "/";
    case KEY_CAPSLOCK:
        return "CapsLock";
    case KEY_F1:
        return "F1";
    case KEY_F2:
        return "F2";
    case KEY_F3:
        return "F3";
    case KEY_F4:
        return "F4";
    case KEY_F5:
        return "F5";
    case KEY_F6:
        return "F6";
    case KEY_F7:
        return "F7";
    case KEY_F8:
        return "F8";
    case KEY_F9:
        return "F9";
    case KEY_F10:
        return "F10";
    case KEY_F11:
        return "F11";
    case KEY_F12:
        return "F12";
    case KEY_PRINTSCREEN:
        return "Print Screen";
    case KEY_SCROLLLOCK:
        return "Scroll Lock";
    case KEY_PAUSE :
        return "Pause";
    case KEY_INSERT:
        return "Insert";
    case KEY_HOME:
        return "Home";
    case KEY_PAGEUP:
        return "Page Up";
    case KEY_DELETE:
        return "Delete";
    case KEY_END:
        return "End";
    case KEY_PAGEDOWN:
        return "Page Down";
    case KEY_NUMLOCK:
        return "Num Lock";
    case KEY_KP_DIVIDE:
        return "KeyPad /";
    case KEY_KP_MULTIPLY:
        return "KeyPad *";
    case KEY_KP_MINUS:
        return "KeyPad -";
    case KEY_KP_PLUS:
        return "KeyPad +";
    case KEY_KP_ENTER:
        return "KeyPad  Enter";
    case KEY_KP_0:
        return "KeyPad 0";
    case KEY_KP_1:
        return "KeyPad 1";
    case KEY_KP_2:
        return "KeyPad 2";
    case KEY_KP_3:
        return "KeyPad 3";
    case KEY_KP_4:
        return "KeyPad 4";
    case KEY_KP_5:
        return "KeyPad 5";
    case KEY_KP_6:
        return "KeyPad 6";
    case KEY_KP_7:
        return "KeyPad 7";
    case KEY_KP_8:
        return "KeyPad 8";
    case KEY_KP_9:
        return "KeyPad 9";
    case KEY_LEFT:
        return "Left";
    case KEY_UP:
        return "UP";
    case KEY_RIGHT:
        return "RIGHT";
    case KEY_DOWN:
        return "DOWN";
    case KEY_KP_PERIOD:
        return "KeyPad .";
    case KEY_APPLICATION:
        return "Application";
    case KEY_F13:
        return "F13";
    case KEY_F14:
        return "F14";
    case KEY_F15:
        return "F15";
    case KEY_F16:
        return "F16";
    case KEY_F17:
        return "F17";
    case KEY_F18:
        return "F18";
    case KEY_F19:
        return "F19";
    case KEY_MENU:
        return "Menu";
    case KEY_MUTE:
        return "Mute";
    case KEY_VOLUMEUP:
        return "Volumne Up";
    case KEY_VOLUMEDOWN:
        return "Volumne Down";
    case KEY_LCTRL:
        return "Left Ctrl";
    case KEY_LSHIFT:
        return "Left Shift";
    case KEY_LALT:
        return "Left Alt";
    case KEY_LGUI:
        return "Left Window/Command";
    case KEY_RCTRL:
        return "Right Ctrl";
    case KEY_RSHIFT:
        return "Right Shift";
    case KEY_RALT:
        return "Right Alt";
    case KEY_RGUI:
        return "Right Window/Command";
    case KEY_AUDIONEXT:
        return "Audio Next";
    case KEY_AUDIOPREV:
        return "Audio Prev";
    case KEY_AUDIOSTOP:
        return "Audio Stop";
    case KEY_AUDIOPLAY:
        return "Audio Play";
    case KEY_AUDIOMUTE:
        return "Audio Mute";
    case KEY_MEDIASELECT:
        return "Media Select";
    default:
        break;

    }

    return "UnknownKey";
}

void KeyboardMapsUtil::trim(string& s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
        }));
}


