#pragma once

#include "../imgui/imgui.h"
#include "../Hosting.h"
#include "../globals/AppIcons.h"
#include "../globals/AppFonts.h"
#include "TitleTooltipWidget.h"
#include "parsec.h"
#include "IconButton.h"
#include "../KeyboardMapsUtil.h"
#include "Widget.h"

#define COLUMN_1_SIZE 150

class KeyboardMapWidget : public Widget
{
	public:
		KeyboardMapWidget(Hosting& hosting);
		bool render(bool &showWindow);
		
	private:
		Hosting& _hosting;

		void InitializeKeyMapSetting(Hosting& _hosting);
		void drawKeySetLine(string labelName, uint16_t& key);
		bool drawCustomKey(int32_t& gamePadKeyType, uint16_t& key, int idx);
		string drawPresetList(string currentPresetName);

		void UpdatePresetName(string newName, string oldName);
		void AddJsonKeyMapItem(MTY_JSON* json, GamePadButtonType_e gamePadKeyType, uint16_t keyVal, int32_t keyType);
		inline void resetPresetName(const char* str, int len);
		void ApplyKeyMap();
		bool SaveKeyFile();

		static void initKeyValues();
		static vector<pair<string, string>> _keyValues;
		static bool _keyNamesInitialized;

		uint16_t _lLeft  = KEY_A;
		uint16_t _lRight = KEY_D;
		uint16_t _lUp    = KEY_W;
		uint16_t _lDown  = KEY_S;
		uint16_t _rLeft  = KEY_LEFT;
		uint16_t _rRight = KEY_RIGHT;
		uint16_t _rUp    = KEY_UP;
		uint16_t _rDown  = KEY_DOWN;
		uint16_t _dLeft  = KEY_KP_4;
		uint16_t _dRight = KEY_KP_6;
		uint16_t _dUp    = KEY_KP_8;
		uint16_t _dDown  = KEY_KP_5;
		uint16_t _a      = KEY_L;
		uint16_t _b      = KEY_O;
		uint16_t _x      = KEY_K;
		uint16_t _y      = KEY_I;
		uint16_t _lB     = KEY_Q;
		uint16_t _rB     = KEY_P;
		uint16_t _lT     = KEY_F;
		uint16_t _rT     = KEY_J;
		uint16_t _back   = KEY_BACKSPACE;
		uint16_t _start  = KEY_ENTER;
		uint16_t _lThumb = KEY_C;
		uint16_t _rThumb = KEY_M;

		char _presetNameBuffer[PRESET_NAME_LEN]= {0,};


};


