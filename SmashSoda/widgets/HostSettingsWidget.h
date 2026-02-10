#pragma once

#define _WINSOCKAPI_

#include "../imgui/imgui.h"
#include "../ImGui/imform.h"
#include "../Hosting.h"
#include "../AudioTools.h"
#include "Debouncer.h"
#include "../globals/AppIcons.h"
#include "../globals/AppFonts.h"
#include "ToggleIconButtonWidget.h"
#include "TitleTooltipWidget.h"
#include "TooltipWidget.h"
#include "ConfirmPopupWidget.h"
#include "IconButton.h"
#include "IntRangeWidget.h"
#include "BoolButtonWidget.h"
#include "AudioControlWidget.h"
#include <chrono>
#include <functional>
#include "../Core/Cache.h"
#include "Widget.h"
#include "../GameData.h"

class HostSettingsWidget : public Widget
{
public:
	HostSettingsWidget(Hosting& hosting, function<void(bool)> onHostRunningStatusCallback);
	bool render(bool& showWindow, HWND& hwnd);
	void renderGeneral(HWND& hwnd);
	void renderAdvanced();
	void renderAudio();
	void updateSecretLink();

	const ImVec2 DEFAULT_BUTTON_SIZE = ImVec2(40, 40);
	const uint8_t LINK_COMPATIBLE_SECRET_SIZE = 9;

private:
	void savePreferences();
	bool validateSettings();
	bool isDirty();
	float lerp(float val1, float val2, float t = 0.11f);
	float easing(float t);

	// Dependency injection
	Hosting& _hosting;
	AudioIn& _audioIn;
	AudioOut& _audioOut;

	// Imported Widgets
	ToggleIconButtonWidget _publicRoomBtn, _playBtn;

	// Attributes
	bool _updated = false;

	char _gameID[GAME_ID_LEN];
	char _roomName[255];
	char _gameName[255];
	char _description[512];
	char _secret[HOST_SECRET_LEN];
	char _secretLink[128];
	bool _kioskMode;
	char _kioskApplication[256];
	char _kioskParam[256];
	char _shareLink[256];
	char _streamUrl[256];
	int32_t _maxGuests;
	int32_t _repThreshold;
	bool _publicGame;
	bool _latencyLimiter;
	int32_t _latencyLimit;
	size_t _selectedArtwork = -1;
	size_t _selectedTheme = 0;
	size_t _selectedCategory = 0;

	// Hotseat
	bool _hotseat;
	bool _hotseatSlotMatch;
	int32_t _hotseatSeats;
	int32_t _hotseatTime;
	bool _hotseatAFK;
	int32_t _hotseatAFKTime;
	bool _hotseatPause;

	int _micVolume = 80;
	int _speakersVolume = 30;

	string _libraryGame;

	// Popups
	string _popupTitle = "Start Hosting";
	string _popupMessage = "Do you want to launch your room?";
	string _validateError = "";
	bool _showErrorPopup = false;

	// Validation errors
	string _roomNameError = "";

	const unsigned int DEBOUNCE_TIME_MS = 1000;
	function<void(bool)> _onHostRunningStatusCallback;

	string _postThemes[6] = { "Default", "Dark", "Blue", "Green", "Red", "FullImage"};
	string _postCategories[12] = { "All", "Fighting", "Party", "Platformer", "Puzzle", "Racing", "RPG", "Shooter", "Simulation", "Sports", "Strategy" };
	const std::vector<std::pair<std::string, std::string>> TagOptions = {
		{"Action", "Action"},
		{"Adventure", "Adventure"},
		{"Arcade", "Arcade"},
		{"Backseat Gamer", "Backseat Gamer"},
		{"Battle Royale", "Battle Royale"},
		{"Beginner Friendly", "Beginner Friendly"},
		{"Best of 2", "Best of 2"},
		{"Best of 3", "Best of 3"},
		{"Best of 5", "Best of 5"},
		{"Double Elimination", "Double Elimination"},
		{"Board Game", "Board Game"},
		{"Bullet Hell", "Bullet Hell"},
		{"Card Game", "Card Game"},
		{"Casual", "Casual"},
		{"Chat", "Chat"},
		{"Co-op", "Co-op"},
		{"Competitive", "Competitive"},
		{"Educational", "Educational"},
		{"Fighting", "Fighting"},
		{"First Person Shooter", "First Person Shooter"},
		{"Hotseat", "Hotseat"},
		{"Horror", "Horror"},
		{"Idle", "Idle"},
		{"Indie", "Indie"},
		{"King of the Hill", "King of the Hill"},
		{"Metroidvania", "Metroidvania"},
		{"MMO", "MMO"},
		{"MOBA", "MOBA"},
		{"Multiplayer", "Multiplayer"},
		{"Music", "Music"},
		{"Open World", "Open World"},
		{"Party Game", "Party Game"},
		{"Platform Fighter", "Platform Fighter"},
		{"Platformer", "Platformer"},
		{"Puzzle", "Puzzle"},
		{"Racing", "Racing"},
		{"Retro", "Retro"},
		{"Rhythm", "Rhythm"},
		{"Role-Playing Game", "Role-Playing Game"},
		{"Roguelike", "Roguelike"},
		{"Sandbox", "Sandbox"},
		{"Shooter", "Shooter"},
		{"Side-scroller", "Side-scroller"},
		{"Simulation", "Simulation"},
		{"Single Elimination", "Single Elimination"},
		{"Single Player", "Single Player"},
		{"Sports", "Sports"},
		{"Stealth", "Stealth"},
		{"Strategy", "Strategy"},
		{"Survival", "Survival"},
		{"Swiss", "Swiss"},
		{"Third Person Shooter", "Third Person Shooter"},
		{"Tournament", "Tournament"},
		{"Tower Defense", "Tower Defense"},
		{"Trivia", "Trivia"},
		{"Turn-based", "Turn-based"},
		{"Twin Stick Shooter", "Twin Stick Shooter"},
		{"Variety", "Variety"},
		{"Visual Novel", "Visual Novel"}
	};
};




