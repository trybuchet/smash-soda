#pragma once

#define _WINSOCKAPI_

#include "XBoxGamepad.h"
#include "DualshockGamepad.h"
#include <Windows.h>
#include <Xinput.h>
#include "ViGEm/Client.h"
#include <vector>
#include <iostream>
#include <algorithm>
#include <functional>
#include <thread>
#include "Models/GuestData.h"
#include "KeyboardMaps.h"
#include "GuestList.h"
#include "services/Hotseat.h"
#include "Keymap.h"

using namespace std;

#define GAMEPAD_LIMIT_NOT_FOUND -1

class GamepadClient
{
public:
	// Singleton accessor (Meyer's singleton pattern)
	static GamepadClient& instance;

	enum class PICK_REQUEST
	{
		OK,
		DISCONNECTED,
		SAME_USER,
		TAKEN,
		EMPTY_HANDS,
		LIMIT_BLOCK,
		OUT_OF_RANGE,
		PUPPET
	};

	class GuestPreferences
	{
	public:
		GuestPreferences()
			: userID(0), padLimit(1), mirror(false), ignoreDeviceID(true)
		{}
		GuestPreferences(uint32_t userID, int padLimit = 1, bool mirror = false, bool ignoreDeviceID = true)
			: userID(userID), padLimit(padLimit), mirror(mirror), ignoreDeviceID(ignoreDeviceID)
		{}

		uint32_t userID = 0;
		int padLimit = 1;
		bool mirror = false;
		bool ignoreDeviceID = true;
	};

	~GamepadClient();
	void setParsec(ParsecDSO* parsec);
	bool init();
	AGamepad* createGamepad(AGamepad::Type type = AGamepad::Type::XBOX, bool connectPad = false);
	void createAllGamepads();
	void createGamepads(int xboxPadCount, int dsPadCount);
	void connectAllGamepads();
	void disconnectAllGamepads();
	void releaseGamepads();
	void sortGamepads();
	void resize(size_t xboxCount, size_t dualshockCount);
	void resetAll(bool& resetting);
	void toggleLock();
	void toggleLockButtons();
	void toggleLockGamepad(int index);
	void toggleLockButtonsGamepad(int index);
	AGamepad* connectNextGamepad();
	void release();
	AGamepad* getGamepad(int index);
	int clearAFK(GuestList &guests);

	bool connect(int gamepadIndex);
	bool disconnect(int gamepadIndex);
	bool clearOwner(int gamepadIndex);
	bool strip(int guestUserID);
	bool stripAll();

	bool sendMessage(Guest guest, ParsecMessage message);
	bool sendButtonMessageForPad(AGamepad* gamepad, ParsecGamepadButtonMessage& button);
	int onQuit(Guest &guest);
	void setLimit(uint32_t guestUserId, uint8_t padLimit);
	bool toggleMirror(uint32_t guestUserID);
	bool toggleIgnoreDeviceID(uint32_t guestUserID);
	const PICK_REQUEST pick(Guest guest, int gamepadIndex);
	bool findPreferences(uint32_t guestUserID, function<void(GuestPreferences&)> callback);

	bool isBusy();
	
	vector<AGamepad*> gamepads;
	vector<GuestPreferences> guestPreferences;

	bool lock = false;
	bool lockButtons = false;
	bool isPuppetMaster = false;
	bool isSlave = false;

	bool canDrop = true;

	KeyboardMap& getKeyMap() { return this->_keyboardMap; }// -- CodeSomnia Add --
	bool mapKeyboard(uint32_t userID, string button);

private:

	bool sendGamepadStateMessage(ParsecGamepadStateMessage& gamepadState, Guest& guest, int& slots, GuestPreferences prefs = GuestPreferences());
	bool sendGamepadAxisMessage(ParsecGamepadAxisMessage& gamepadAxis, Guest& guest, int& slots, GuestPreferences prefs = GuestPreferences());
	bool sendGamepadButtonMessage(ParsecGamepadButtonMessage& gamepadButton, Guest& guest, int& slots, GuestPreferences prefs = GuestPreferences());
	bool sendKeyboardMessage(ParsecKeyboardMessage& keyboard, Guest& guest, int& slots, GuestPreferences prefs = GuestPreferences());

	void setMirror(uint32_t guestUserID, bool mirror);
	void setIgnoreDeviceID(uint32_t guestUserID, bool ignoreDeviceID);
	bool tryAssignGamepad(Guest guest, uint32_t padId, int currentSlots, bool isKeyboard, GuestPreferences prefs = GuestPreferences());
	bool isRequestState(ParsecMessage message);
	bool isRequestButton(ParsecMessage message);
	bool isRequestKeyboard(uint32_t userID, ParsecMessage message);

	void reduce(function<void(AGamepad*)> func);
	bool reduceUntilFirst(function<bool(AGamepad*)> func);

	XINPUT_STATE toXInput(ParsecGamepadStateMessage& state, XINPUT_STATE previousState, GuestPreferences& prefs, uint32_t userID);
	XINPUT_STATE toXInput(ParsecGamepadButtonMessage& button, XINPUT_STATE previousState, GuestPreferences& prefs, uint32_t userID);
	XINPUT_STATE toXInput(ParsecGamepadAxisMessage& axis, XINPUT_STATE previousState, GuestPreferences& prefs, uint32_t userID);
	XINPUT_STATE toXInput(ParsecKeyboardMessage& key, AGamepad::Keyboard& keyboard, XINPUT_STATE previousState, GuestPreferences& prefs, uint32_t userID);


	PVIGEM_CLIENT _client;
	ParsecDSO* _parsec;

	thread _resetAllThread;
	thread _createGamepadsThread;

	KeyboardMap _keyboardMap;

	bool _isBusy = false;

	bool _isMapping = false;
	uint32_t _mappingID = 0;
	string _mappingButton = "";

	int _xboxCount;
	int _dsCount;
};


