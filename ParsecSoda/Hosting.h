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
#include "Chatbot/ChatBot.h"
#include "ChatLog.h"
#include "Helpers/Stringer.h"
#include "AudioIn.h"
#include "AudioOut.h"
#include "AudioMix.h"
#include "GamepadClient.h"
#include "GuestList.h"
#include "MetadataCache.h"
#include "CompilerDirectives.h"
#include "Helpers/Stopwatch.h"
#include "MasterOfPuppets.h"
#include "ButtonLock.h"
#include "Helpers/Debouncer.h"
#include "Modules/Macro.h"
#include "Modules/Hotseat.h"
#include "Modules/WebSocket.h"
#include "Modules/ProcessMan.h"
#include "Modules/Tournament.h"
#include "Modules/AutoMod.h"
#include "Modules/Arcade.h"

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
	bool& isGamepadLock();
	bool& isGamepadLockButtons();
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
	GuestList& getGuestList();
	vector<Guest>& getPlayingGuests();
	vector<Guest>& getRandomGuests();
	Guest& getGuest(uint32_t id);
	int getGuestIndex(uint32_t id);
	bool guestExists(uint32_t id);
	vector<Guest>& getGuestsAfterGuest(uint32_t targetGuestID, int count, bool ignoreSpectators);
	vector<GuestData>& getGuestHistory();
	MyMetrics getMetrics(uint32_t id);
	vector<AGamepad*>& getGamepads();
	GamepadClient& getGamepadClient();
	MasterOfPuppets& getMasterOfPuppets();
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

	LockedGamepadState _lockedGamepad;
	void updateButtonLock(LockedGamepadState lockedGamepad);

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
	bool _latencyLimitEnabled = false;
	unsigned int _latencyLimitThreshold = 0;
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
	bool handleGuestIpAddress(Guest& guest);
	void handleGuestConnecting(Guest& guest, ParsecStatus& status);
	void handleGuestConnected(Guest& guest, ParsecGuestState& state, ParsecStatus& status);
	void handleGuestFailure(Guest& guest, ParsecGuestState& state, ParsecStatus& status);
	void handleGuestDisconnection(Guest& guest, ParsecGuestState& state, ParsecStatus& status);

	void addNewGuest(Guest guest);
	void handleNewGuests();

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
	GamepadClient _gamepadClient;
	GuestList _guestList;
	MasterOfPuppets _masterOfPuppets;

	ParsecDSO* _parsec;
	ParsecHostConfig _hostConfig;
	ParsecSession _parsecSession;
	ParsecStatus _parsecStatus;
	Guest _host;
	TierList _tierList;
	Macro _macro;
	Hotseat _hotseat;
	Tournament _tournament;

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
	thread _autorunSmashSodaThread;

	mutex _mediaMutex;
	mutex _inputMutex;
	mutex _eventMutex;
	mutex _latencyMutex;
	mutex _smashSodaMutex;
	mutex _gamepadMutex;

	string _roomToken;

	const int _mickeyID = 1693946;
};
