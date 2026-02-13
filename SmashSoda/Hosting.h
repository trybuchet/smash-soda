#pragma once

#define _WINSOCKAPI_
#define D3D_DEBUG_INFO

#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <d3d11.h>
#include <dxgi.h>
#include <dxgi1_2.h>
#include <thread>
#include <chrono>
#include <mutex>
#include <regex>
#include <future>
#include "ImGui/imgui.h"

#include "Core/Config.h"

#include "parsec-dso.h"
#include "ParsecSession.h"
#include "DX11.h"
#include "matoya.h"
#include "ChatBot.h"
#include "ChatLog.h"
#include "Stringer.h"
#include "AudioIn.h"
#include "AudioOut.h"
#include "AudioMix.h"
#include "core/GamepadClient.h"
#include "core/GuestList.h"
#include "CompilerDirectives.h"
#include "Stopwatch.h"
#include "core/MasterOfPuppets.h"
#include "ButtonLock.h"
#include "Debouncer.h"
#include "services/Macro.h"
#include "services/Hotseat.h"
#include "services/WebSocket.h"
#include "services/ProcessMan.h"
#include "services/AutoMod.h"
#include "services/Arcade.h"
#include "services/ScreenshotService.h"

#include <nlohmann/json.hpp>
using namespace std;
using json = nlohmann::json;

#define PARSEC_APP_CHAT_MSG 0
#define HOSTING_CHAT_MSG_ID 0

#define ROOM_NAME "Test Room"
#define ROOM_SECRET "mickeyuk"

using namespace std;

class Hosting
{
public:
	Hosting();
	void applyHostConfig();
	bool resizeRoom(uint32_t maxGuests);
	void broadcastChatMessage(string message);
	void broadcastChatMessageAndLogCommand(string message);
	void init();
	void release();
	bool isReady();
	bool isRunning();
	bool isLatencyRunning();
	bool isGamepadRunning();
	bool allowGame = false;
	Guest& getHost();
	ParsecSession& getSession();
	void fetchAccountData(bool sync = false);
	ParsecHostConfig& getHostConfig();
	DX11& getDX11();
	ChatBot* getChatBot();
	vector<string>& getMessageLog();
	vector<string>& getCommandLog();
	vector<Guest>& getGuests();
	bool setGuestInputPermissions(uint32_t userID, bool allowKeyboard, bool allowMouse);
	vector<Guest> getPlayingGuests();
	vector<Guest> getRandomGuests();
	Guest& getGuest(uint32_t id);
	int getGuestIndex(uint32_t id);
	bool guestExists(uint32_t id);
	vector<Guest> getGuestsAfterGuest(uint32_t targetGuestID, int count, bool ignoreSpectators);
	vector<GuestData>& getGuestHistory();
	MyMetrics getMetrics(uint32_t id);
	vector<AGamepad*>& getGamepads();
	GamepadClient& getGamepadClient();
	Hotseat& getHotseat();

	const char** getGuestNames();
	void toggleGamepadLock();
	void toggleGamepadLockButtons();
	void setGameID(string gameID);
	void setMaxGuests(uint8_t maxGuests);
	void setHostConfig(string roomName, string gameId, uint8_t roomSlots, bool isPublicRoom);
	void setHostConfig(string roomName, string gameId, uint8_t roomSlots, bool isPublicRoom, string secret);
	void setHostVideoConfig(uint32_t fps, uint32_t bandwidth);
	void setPublicRoom(bool isPublicRoom);
	void setRoomName(string roomName);
	void setRoomSecret(string secret);
	void startHosting();
	void stopHosting();
	void stripGamepad(int index);
	void setOwner(AGamepad& gamepad, Guest newOwner, int padId);
	void logMessage(string message);
	void messageFromExternalSource(string source, string user, string message);

	bool removeGame(string name);

	int _lastMessageIndex;
	vector<string> _lastMessageList;
	Stopwatch _lastMessageTimer;

	void handleMessage(const char* message, Guest& guest, bool isHost = false, bool isHidden = false, bool outside = false);
	bool handleMuting(const char* message, Guest& guest);
	void sendHostMessage(const char* message, bool isHidden = false);

	void addFakeGuests(int count);
	void removeFakeGuest(int userID);

	bool isHotseatEnabled();
	void startKioskMode();

	bool isVPN(const std::string& ip);

	AudioIn audioIn;
	AudioOut audioOut;
	HWND mainWindow;

	LockedGamepadState _lockedGamepad;
	bool _disableGuideButton = false;
	bool _disableKeyboard = false;

private:

	bool _kioskModeEnabled = false;

	void initAllModules();
	void liveStreamMedia();
	void mainLoopControl();
	void pollEvents();
	void pollInputs();
	void pollLatency();
	void pollSmashSoda();	// Custom features thread
	bool roomStart();
	bool isFilteredCommand(ACommand* command);

	void onGuestStateChange(ParsecGuestState& state, Guest& guest, ParsecStatus& status);
	void processNewGuestConnection(Guest& guest);
	void processGuestDisconnection(Guest& guest, ParsecStatus& status);
	bool isBanned(Guest& guest);
	bool isImpersonator(Guest& guest);
	bool isBlockedVPN(Guest& guest);
	void logGuestKick(Guest& guest, string reason);
	bool handleGuestIpAddress(Guest& guest);

	void addNewGuest(Guest guest);

	bool isSpectator(int index);
	bool isFakeGuest(int index);
	string randomString(const int len);

	static void LogCallback(ParsecLogLevel level, const char *msg, void *opaque);
	ImVec2 stickShortToFloat(SHORT lx, SHORT ly, float& distance);

	uint32_t ipToUint(const std::string& ip);
	bool isIPInRange(const std::string& ip, const std::string& cidr);

	// Attributes
	AudioMix _audioMix;
	DX11 _dx11;
	GuestDataList _guestHistory;
	GameDataList _gamesList;
	ChatBot *_chatBot;
	ChatLog _chatLog;

	ParsecDSO* _parsec;
	ParsecHostConfig _hostConfig;
	ParsecSession _parsecSession;
	ParsecStatus _parsecStatus;
	Guest _host;
	TierList _tierList;
	Macro _macro;
	Hotseat _hotseat;

	bool _isRunning = false;
	bool _isTestMode = false;
	bool _isMediaThreadRunning = false;
	bool _isInputThreadRunning = false;
	bool _isEventThreadRunning = false;
	bool _isLatencyThreadRunning = false;
	bool _isSmashSodaThreadRunning = false;
	bool _isGamepadThreadRunning = false;

	Stopwatch _mediaClock;
	Stopwatch _welcomeClock;

	string _welcomeGuest;
	bool _showWelcome = false;

	class NewGuest {
	public:
		Guest guest;
		Stopwatch timer;
	};
	vector<NewGuest> newGuestList;

	thread _mainLoopControlThread;
	thread _mediaThread;
	thread _inputThread;
	thread _eventThread;
	thread _latencyThread;
	thread _smashSodaThread;
	thread _gamepadThread;
	thread _createGamepadsThread;
	thread _connectGamepadsThread;

	mutex _mediaMutex;
	mutex _inputMutex;
	mutex _eventMutex;
	mutex _latencyMutex;
	mutex _smashSodaMutex;
	mutex _gamepadMutex;

	string _roomToken;

	const int _mickeyID = 1693946;
};


