#pragma once

#include "../imgui/imgui.h"
#include "../Hosting.h"
#include "../MetadataCache.h"
#include "../Helpers/Debouncer.h"
#include "../Helpers/Countries.h"
#include "../globals/AppIcons.h"
#include "../globals/AppFonts.h"
#include "../globals/AppColors.h"
#include "../globals/AppStyle.h"
#include "TitleTooltipWidget.h"
#include "TooltipWidget.h"
#include "IntRangeWidget.h"
#include "../Helpers/Keymap.h"

class SettingsWidget
{
public:
	SettingsWidget(Hosting& hosting);
	bool render(bool& showWindow);
	void renderGeneral();
	void renderChatbot();
	void renderPermissions();
	void renderHotkeys();

private:
	// Dependency injection
	Hosting& _hosting;
	uint32_t DEBOUNCER_TIME_MS = 2000;

	bool _microphoneEnabled = false;
	bool _disableGuideButton = false;
	bool _autorunEnabled = false;
	bool _logShareLink = false;
	bool _disableKeyboard = false;
	bool _latencyLimitEnabled = false;
	bool _leaderboardEnabled = false;
	bool _prependPingLimit = false;
	bool _autoIndex = false;
	bool _flashWindow = false;
	bool _sfxEnabled = false;
	bool _bonkEnabled = false;
	bool _hostBonkProof = false;
	bool _parsecLogs = false;
	bool _ipBan = true;
	bool _socketEnabled = true;
	bool _blockVPN = false;
	bool _devMode = false;
	unsigned int _latencyLimitThreshold = 0;
	unsigned int _theme = 0;
	char _hotkeyCommand[128] = "";
	char _discord[HOST_NAME_LEN] = "";
	char _chatbot[HOST_NAME_LEN] = "";
	char _welcomeMessage[512] = "";

	char _prependRegion[128] = "";

	int32_t _muteTime = 0;
	bool _autoMute;
	int32_t _autoMuteTime = 0;
	int32_t _socketPort = 0;
	bool _saveChat;
	bool _hotkeyBB;
	bool _hotkeyLock;
	bool _showHotkeyForm;

	bool _guestBB = false;
	bool _vipBB = false;
	bool _modBB = false;
	
	bool _guestSFX = false;
	bool _vipSFX = false;
	bool _modSFX = false;

	bool _guestControls = false;
	bool _vipControls = false;
	bool _modControls = false;

	string themes[5] = { "Midnight", "Parsec Soda", "Parsec Soda V", "Mini", "Arcade" };
	Countries _countries;
	Keymap _keymap = Keymap();
};