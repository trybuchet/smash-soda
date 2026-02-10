#pragma once

#include "parsec.h"
#include "matoya.h"
#include <string>
#include <Windows.h>
#include <ShlObj.h>

# define GAMEPAD_KEY_COUNT 25
# define PRESET_NAME_LEN 64
/*
	CodeSomnia
	: Å°º¸µå ¸ÅÇÎÀ» µ½´Â ±âº» Å¸ÀÔµé°ú
*/

using namespace std;

typedef enum KeyType_e
{
	NONE,
	ORIGIN,
	CUSTOM
}KeyType_e;

typedef enum GamePadButtonType_e {
	None  ,
	LLeft ,
	LRight,
	LUp   ,
	LDown ,
	RLeft ,
	RRight,
	RUp   ,
	RDown ,
	DLeft ,
	DRight,
	DUp   ,
	DDown ,
	A     ,
	B     ,
	X     ,
	Y     ,
	LB    ,
	RB    ,
	LT    ,
	RT    ,
	Back  ,
	Start ,
	LThumb,
	RThumb,
	
}GamePadButtonType_e;

//CodeSomnia : ¼³Á¤ Çã¿ëÇÏ´Â Å°º¸µå Å° ¸ñ·Ï. ÃÖÀûÈ­¸¦ À§ÇØ ¹Ì¸® ¸Þ¸ð¸®¿¡ ¿Ã·ÁµÎ°í »ç¿ë
static uint16_t _allowedSettionKeys[] = {
			KEY_A				  ,
			KEY_B				  ,
			KEY_C				  ,
			KEY_D				  ,
			KEY_E				  ,
			KEY_F				  ,
			KEY_G				  ,
			KEY_H				  ,
			KEY_I				  ,
			KEY_J				  ,
			KEY_K				  ,
			KEY_L				  ,
			KEY_M				  ,
			KEY_N				  ,
			KEY_O				  ,
			KEY_P				  ,
			KEY_Q				  ,
			KEY_R				  ,
			KEY_S				  ,
			KEY_T				  ,
			KEY_U				  ,
			KEY_V				  ,
			KEY_W				  ,
			KEY_X				  ,
			KEY_Y				  ,
			KEY_Z				  ,
			KEY_1				  ,
			KEY_2				  ,
			KEY_3				  ,
			KEY_4				  ,
			KEY_5				  ,
			KEY_6				  ,
			KEY_7				  ,
			KEY_8				  ,
			KEY_9				  ,
			KEY_0				  ,
			KEY_ENTER			  ,
			KEY_ESCAPE			  ,
			KEY_BACKSPACE		  ,
			KEY_TAB				  ,
			KEY_SPACE			  ,
			KEY_MINUS			  ,
			KEY_EQUALS			  ,
			KEY_LBRACKET		  ,
			KEY_RBRACKET		  ,
			KEY_BACKSLASH		  ,
			KEY_SEMICOLON		  ,
			KEY_APOSTROPHE		  ,
			KEY_BACKTICK		  ,
			KEY_COMMA			  ,
			KEY_PERIOD			  ,
			KEY_SLASH			  ,
			KEY_CAPSLOCK		  ,
			KEY_F1				  ,
			KEY_F2				  ,
			KEY_F3				  ,
			KEY_F4				  ,
			KEY_F5				  ,
			KEY_F6				  ,
			KEY_F7				  ,
			KEY_F8				  ,
			KEY_F9				  ,
			KEY_F10				  ,
			KEY_F11				  ,
			KEY_F12				  ,
			KEY_PRINTSCREEN		  ,
			KEY_SCROLLLOCK		  ,
			KEY_PAUSE			  ,
			KEY_INSERT			  ,
			KEY_HOME			  ,
			KEY_PAGEUP			  ,
			KEY_DELETE			  ,
			KEY_END				  ,
			KEY_PAGEDOWN		  ,
			KEY_RIGHT			  ,
			KEY_LEFT			  ,
			KEY_DOWN			  ,
			KEY_UP				  ,
			KEY_NUMLOCK			  ,
			KEY_KP_DIVIDE		  ,
			KEY_KP_MULTIPLY		  ,
			KEY_KP_MINUS		  ,
			KEY_KP_PLUS			  ,
			KEY_KP_ENTER		  ,
			KEY_KP_1			  ,
			KEY_KP_2			  ,
			KEY_KP_3			  ,
			KEY_KP_4			  ,
			KEY_KP_5			  ,
			KEY_KP_6			  ,
			KEY_KP_7			  ,
			KEY_KP_8			  ,
			KEY_KP_9			  ,
			KEY_KP_0			  ,
			KEY_KP_PERIOD		  ,
			KEY_APPLICATION		  ,
			KEY_F13				  ,
			KEY_F14				  ,
			KEY_F15				  ,
			KEY_F16				  ,
			KEY_F17				  ,
			KEY_F18				  ,
			KEY_F19				  ,
			KEY_MENU			  ,
			KEY_MUTE			  ,
			KEY_VOLUMEUP		  ,
			KEY_VOLUMEDOWN		  ,
			KEY_LCTRL			  ,
			KEY_LSHIFT			  ,
			KEY_LALT			  ,
			KEY_LGUI			  ,
			KEY_RCTRL			  ,
			KEY_RSHIFT			  ,
			KEY_RALT			  ,
			KEY_RGUI			  ,
			KEY_AUDIONEXT		  ,
			KEY_AUDIOPREV		  ,
			KEY_AUDIOSTOP		  ,
			KEY_AUDIOPLAY		  ,
			KEY_AUDIOMUTE		  ,
			KEY_MEDIASELECT		  ,	
};

typedef struct KeyMapInfo
{
	int32_t gamePadKeyType;
	uint16_t keyVal;
	int32_t keyType;
}KeyMapInfo;

typedef struct GamePadKeyPair_t {
	string name;
	GamePadButtonType_e buttonType;
}GamePadKeyPair_t;

//CodeSomnia : °¢ °ÔÀÓÅ°ÆÐµå¿¡ ¸ÅÄªµÇ´Â Å°°ªµéÀ» ÀÌ¸§°ú Å¸ÀÔÀ¸·Î
//             ±¸ºÐÇÏ¿© ¹Ì¸® ¸Þ¸ð¸®¿¡ ¿Ã·Á »ç¿ë.
static GamePadKeyPair_t _allowedCustomKeys[GAMEPAD_KEY_COUNT] = {
	{"None"    ,GamePadButtonType_e::None },
	{"LLeft"   ,GamePadButtonType_e::LLeft },
	{"LRight"  ,GamePadButtonType_e::LRight},
	{"LUp"     ,GamePadButtonType_e::LUp   },
	{"LDown"   ,GamePadButtonType_e::LDown },
	{"RLeft"   ,GamePadButtonType_e::RLeft },
	{"RRight"  ,GamePadButtonType_e::RRight},
	{"RUp"     ,GamePadButtonType_e::RUp   },
	{"RDown"   ,GamePadButtonType_e::RDown },
	{"DLeft"   ,GamePadButtonType_e::DLeft },
	{"DRight"  ,GamePadButtonType_e::DRight},
	{"DUp"     ,GamePadButtonType_e::DUp   },
	{"DDown"   ,GamePadButtonType_e::DDown },
	{"A"       ,GamePadButtonType_e::A     },
	{"B"       ,GamePadButtonType_e::B     },
	{"X"       ,GamePadButtonType_e::X     },
	{"Y"       ,GamePadButtonType_e::Y     },
	{"LB"      ,GamePadButtonType_e::LB    },
	{"RB"      ,GamePadButtonType_e::RB    },
	{"LT"      ,GamePadButtonType_e::LT    },
	{"RT"      ,GamePadButtonType_e::RT    },
	{"Back"    ,GamePadButtonType_e::Back  },
	{"Start"   ,GamePadButtonType_e::Start },
	{"LThumb"  ,GamePadButtonType_e::LThumb},
	{"RThumb"  ,GamePadButtonType_e::RThumb},
};

class KeyboardMapsUtil {
	public:
		static string getKeyMapSavePath();
		static string getKeyToString(uint16_t keyVal);
		static string getGamePadKeyTypeToString(GamePadButtonType_e keyType);
		static void trim(string& s);

};



