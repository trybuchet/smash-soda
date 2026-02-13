#include "GamepadClient.h"
#include "Config.h"
#include "../Hosting.h"
extern Hosting g_hosting;

// Singleton instance (Meyer's singleton pattern)
static GamepadClient _gamepadClientInstance;
GamepadClient& GamepadClient::instance = _gamepadClientInstance;

// =============================================================
//
//  GAMEPAD ENGINE
// 
// =============================================================
GamepadClient::~GamepadClient()
{
	release();
}

void GamepadClient::setParsec(ParsecDSO* parsec)
{
	this->_parsec = parsec;
}

bool GamepadClient::init()
{
	if (_client != nullptr)
	{
		release();
	}

	_client = vigem_alloc();
	if (_client == nullptr)
	{
		return false;
	}

	const VIGEM_ERROR err = vigem_connect(_client);
	if (!VIGEM_SUCCESS(err))
	{
		return false;
	}

	return true;
}

AGamepad* GamepadClient::createGamepad(AGamepad::Type type, bool connectPad)
{
	if (_client == nullptr)
	{
		return nullptr;
	}

	AGamepad* gamepad = nullptr;
	switch (type)
	{
	case AGamepad::Type::DUALSHOCK:
		gamepad = new DualshockGamepad(_parsec, _client);
		break;
	default:
	case AGamepad::Type::XBOX:
		gamepad = new XBoxGamepad(_parsec, _client);
		break;
	}

	if (gamepad != nullptr) {
		gamepads.push_back(gamepad);
		if (connectPad) {
			gamepad->connect();
		}
	}

	return gamepad;
}

void GamepadClient::createAllGamepads()
{
	_isBusy = true;

	for (uint16_t i = 0; i < Config::cfg.input.xboxPuppetCount; i++)
	{
		this->createGamepad(AGamepad::Type::XBOX);
	}

	for (uint16_t i = 0; i < Config::cfg.input.ds4PuppetCount; i++)
	{
		this->createGamepad(AGamepad::Type::DUALSHOCK);
	}

	sortGamepads();
	_isBusy = false;
	Sleep(200);
}


void GamepadClient::createGamepads(int xboxPadCount, int dsPadCount) {

	_isBusy = true;

	_xboxCount = xboxPadCount;
	_dsCount = dsPadCount;

	Config::cfg.input.xboxPuppetCount = 0;
	Config::cfg.input.ds4PuppetCount = 0;

	_createGamepadsThread = thread([&]() {

		releaseGamepads();
		Sleep(1000);

		_isBusy = true;

		for (uint16_t i = 0; i < _xboxCount; i++) {
			this->createGamepad(AGamepad::Type::XBOX);
			Sleep(100);
		}

		for (uint16_t i = 0; i < _dsCount; i++) {
			this->createGamepad(AGamepad::Type::DUALSHOCK);
			Sleep(100);
		}

		//sortGamepads();
		Sleep(1000);

		std::vector<AGamepad*>::iterator gi = gamepads.begin();
		for (; gi != gamepads.end(); ++gi) {

			(*gi)->connect();
			Sleep(1000);

		}

		Sleep(1000);
		gi = gamepads.begin();
		for (; gi != gamepads.end(); ++gi) {

			(*gi)->refreshIndex();

			//ostringstream test;
			//test << (*gi)->getIndex();
			//g_hosting.sendHostMessage(test.str().c_str());
			
			Sleep(1000);

		}

		_isBusy = false;
		_createGamepadsThread.detach();

	});

}

void GamepadClient::connectAllGamepads() {
	reduce([](AGamepad* pad) {
		pad->connect();
		Sleep(200);
	});
}

void GamepadClient::disconnectAllGamepads()
{
	reduce([](AGamepad* pad) {
		pad->disconnect();
	});
}

void GamepadClient::sortGamepads() {
	std::vector<AGamepad*> sorted = gamepads;
	std::sort(
		sorted.begin(),
		sorted.end(),
		[](const AGamepad* a, const AGamepad* b) {
			if (a->type() == AGamepad::Type::DUALSHOCK && b->type() == AGamepad::Type::XBOX) return false;
			if (a->type() == AGamepad::Type::XBOX && b->type() == AGamepad::Type::DUALSHOCK) return true;
			return a->getIndex() < b->getIndex();
		}
	);
	gamepads = sorted;
}

void GamepadClient::resize(size_t xboxCount, size_t dualshockCount) {
	if (_isBusy) return;

	size_t xi = 0, di = 0;
	vector<AGamepad*> newGamepads;

	lock = true;
	lockButtons = true;
	_isBusy = true;

	reduce([&xi, &di, &xboxCount, &dualshockCount, &newGamepads](AGamepad* pad) {
		switch (pad->type())
		{
		case AGamepad::Type::XBOX:
			if (xi < xboxCount) {
				newGamepads.push_back(pad);
				xi++;
			}
			else {
				pad->release();
			}
			break;
		case AGamepad::Type::DUALSHOCK:
			if (di < dualshockCount) {
				newGamepads.push_back(pad);
				di++;
			}
			else {
				pad->release();
			}
			break;
		default:
			break;
		}
	});

	if (xi < xboxCount)
	{
		for (size_t i = 0; i < xboxCount-xi; i++)
		{
			newGamepads.push_back(this->createGamepad(AGamepad::Type::XBOX));
		}
	}
	if (di < dualshockCount)
	{
		for (size_t i = 0; i < dualshockCount-di; i++)
		{
			newGamepads.push_back(this->createGamepad(AGamepad::Type::DUALSHOCK));
		}
	}

	gamepads.clear();
	gamepads = newGamepads;
	sortGamepads();

	_isBusy = false;
	lock = false;
	lockButtons = false;
}

void GamepadClient::resetAll(bool& resetting)
{
	_resetAllThread = thread([&]() {
		lock = true;
		lockButtons = true;
		release();
		init();
		createAllGamepads();
		connectAllGamepads();
		lock = false;
		lockButtons = false;
		_resetAllThread.detach();
		resetting = false;
	});
}

void GamepadClient::toggleLock()
{
	lock = !lock;
	reduce([&](AGamepad* pad) {
		pad->clearState();
	});
}

void GamepadClient::toggleLockButtons()
{
	lockButtons = !lockButtons;
	reduce([&](AGamepad* pad) {
		pad->clearState();
	});
}


AGamepad* GamepadClient::connectNextGamepad()
{
	AGamepad* rv;

	bool success = reduceUntilFirst([&](AGamepad* pad) {
		if (pad->connect())
		{
			rv = pad;
			return true;
		}
		return false;
	});

	if (success)
	{
		return rv;
	}
	
	return nullptr;
}

bool GamepadClient::disconnect(int gamepadIndex)
{
	if (gamepadIndex < 0 || gamepadIndex > gamepads.size())
	{
		return false;
	}

	bool rv = gamepads[gamepadIndex]->disconnect();
	return rv;
}

bool GamepadClient::connect(int gamepadIndex)
{
	if (gamepadIndex < 0 || gamepadIndex > gamepads.size())
	{
		return false;
	}

	bool rv = gamepads[gamepadIndex]->connect();
	return rv;
}

bool GamepadClient::clearOwner(int gamepadIndex)
{
	if (gamepadIndex >= 0 && gamepadIndex < gamepads.size()) {

		Hotseat::instance.pauseUser(gamepads[gamepadIndex]->owner.guest.userID);
		gamepads[gamepadIndex]->clearOwner();
		
		return true;
	}
	return false;
}

void GamepadClient::release()
{
	releaseGamepads();
	if (_client != nullptr)
	{
		vigem_disconnect(_client);
		vigem_free(_client);
		_client = nullptr;
	}
}


AGamepad* GamepadClient::getGamepad(int index)
{
	if (index >= 0 && index < gamepads.size())
	{
		return gamepads[index];
	}

	return nullptr;
}

int GamepadClient::clearAFK(GuestList &guests)
{
	int clearCount = 0;

	reduce([&](AGamepad* pad) {
		if (pad->isOwned())
		{
			Guest guest;
			if (!guests.find(pad->owner.guest.userID, &guest))
			{
				Hotseat::instance.pauseUser(pad->owner.guest.userID);
				pad->clearOwner();
				clearCount++;
			}
		}
	});

	return clearCount;
}

int GamepadClient::onQuit(Guest& guest) {
	int result = 0;

	Hotseat::instance.pauseUser(guest.userID);

	reduce([&](AGamepad* gamepad) {
		if (gamepad->owner.guest.userID == guest.userID) {
			gamepad->clearOwner();
			result++;
		}
	});

	return result;
}

/// <summary>
/// Snatches a gamepad from a guest.
/// </summary>
/// <param name="guestUserID"></param>
/// <returns></returns>
bool GamepadClient::strip(int guestUserID) {
	bool success = false;

	reduce([&](AGamepad* gamepad) {
		if (gamepad->owner.guest.userID == guestUserID) {
			gamepad->clearOwner();
			success = true;

			Hotseat::instance.pauseUser(guestUserID);
		}
	});

	return success;
}

/// <summary>
/// Strip all gamepads.
/// </summary>
/// <returns></returns>
bool GamepadClient::stripAll() {
	bool success = false;

	reduce([&](AGamepad* gamepad) {
		if (gamepad->isOwned()) {
			const uint32_t ownerUserID = gamepad->owner.guest.userID;
			gamepad->clearOwner();
			success = true;

			Hotseat::instance.pauseUser(ownerUserID);
		}
	});

	return success;
}

void GamepadClient::setLimit(uint32_t guestUserID, uint8_t padLimit)
{
	bool found = findPreferences(guestUserID, [&padLimit](GuestPreferences& prefs) {
		prefs.padLimit = padLimit;
	});

	if (!found)
	{
		guestPreferences.push_back(
			GuestPreferences(guestUserID, padLimit)
		);
	}
}

bool GamepadClient::toggleMirror(uint32_t guestUserID)
{
	bool currentValue = false;

	bool found = findPreferences(guestUserID, [&currentValue](GuestPreferences& prefs) {
		prefs.mirror = !prefs.mirror;
		currentValue = prefs.mirror;
	});

	if(!found)
	{
		GuestPreferences prefs = GuestPreferences(guestUserID, 1, true);
		guestPreferences.push_back(prefs);
		currentValue = prefs.mirror;
	}

	return currentValue;
}

bool GamepadClient::toggleIgnoreDeviceID(uint32_t guestUserID)
{
	bool currentValue = false;

	bool found = findPreferences(guestUserID, [&currentValue](GuestPreferences& prefs) {
		prefs.ignoreDeviceID = !prefs.ignoreDeviceID;
		currentValue = prefs.ignoreDeviceID;
	});

	if (!found)
	{
		GuestPreferences prefs = GuestPreferences(guestUserID, 1, false, false);
		guestPreferences.push_back(prefs);
		currentValue = prefs.ignoreDeviceID;
	}

	return currentValue;
}

const GamepadClient::PICK_REQUEST GamepadClient::pick(Guest guest, int gamepadIndex)
{

	if (gamepadIndex < 0 || gamepadIndex >= gamepads.size())
	{
		return PICK_REQUEST::OUT_OF_RANGE;
	}

	AGamepad* pad = gamepads[gamepadIndex];
	if (!pad->isConnected())
	{
		return PICK_REQUEST::DISCONNECTED;
	}

	if (pad->owner.guest.userID == guest.userID)
	{
		return PICK_REQUEST::SAME_USER;
	}

	if (pad->isOwned())
	{
		return PICK_REQUEST::TAKEN;
	}

	if (isPuppetMaster && pad->isPuppet)
	{
		return PICK_REQUEST::PUPPET;
	}

	GuestPreferences prefs = GuestPreferences(guest.userID);
	int limit = 1;
	bool found = findPreferences(guest.userID, [&limit](GuestPreferences& prefs) {
		limit = prefs.padLimit;
	});

	if (limit <= 0)
	{
		return PICK_REQUEST::LIMIT_BLOCK;
	}

	bool success = reduceUntilFirst([&](AGamepad* gamepad) {
		if (gamepad->owner.guest.userID == guest.userID) {
			if (!Config::cfg.hotseat.enabled || Hotseat::instance.checkUser(guest.userID, guest.name)) {
				pad->clearState();
				pad->copyOwner(gamepad);
				gamepad->clearOwner();
				if (Config::cfg.hotseat.enabled) {
					Hotseat::instance.seatUser(guest.userID, guest.name);
				}
			}
			return true;
		}

		return false;
	});

	if (success)
	{
		return PICK_REQUEST::OK;
	}

	return PICK_REQUEST::EMPTY_HANDS;
}


// =============================================================
//
//  Private
//
// =============================================================

bool GamepadClient::sendMessage(Guest guest, ParsecMessage message)
{
	bool success = false;
	uint32_t padId = GAMEPAD_INDEX_ERROR;
	bool isGamepadRequest = false;
	int slots = 0;
	
	GuestPreferences guestPrefs = GuestPreferences();
	findPreferences(guest.userID, [&guestPrefs](GuestPreferences& prefs) {
		guestPrefs = prefs;
	});

	switch (message.type)
	{
	case MESSAGE_GAMEPAD_STATE:
		padId = message.gamepadState.id;
		isGamepadRequest = isRequestState(message);
		if (sendGamepadStateMessage(message.gamepadState, guest, slots, guestPrefs)) { return true; }
		break;

	case MESSAGE_GAMEPAD_AXIS:
		padId = message.gamepadAxis.id;
		if (sendGamepadAxisMessage(message.gamepadAxis, guest, slots, guestPrefs)) { return true; }
		break;

	case MESSAGE_GAMEPAD_BUTTON:
		padId = message.gamepadButton.id;
		isGamepadRequest = isRequestButton(message);
		if (sendGamepadButtonMessage(message.gamepadButton, guest, slots, guestPrefs)) { return true; }
		break;

	case MESSAGE_KEYBOARD:
		if (g_hosting._disableKeyboard) break;
		padId = 0;
		isGamepadRequest = isRequestKeyboard(guest.userID, message);
		if (sendKeyboardMessage(message.keyboard, guest, slots, guestPrefs)) { return true; }
		break;

	default:
		break;
	}

	if (!success && isGamepadRequest)
	{
		success = tryAssignGamepad(guest, padId, slots, message.type == MESSAGE_KEYBOARD, guestPrefs);
	}

	return success;
}

bool GamepadClient::sendButtonMessageForPad(AGamepad* gamepad, ParsecGamepadButtonMessage& button) {

	GuestPreferences guestPrefs = GuestPreferences();
	XINPUT_STATE xstate = toXInput(button, gamepad->getState(), guestPrefs, gamepad->owner.guest.userID);
	if (Config::cfg.input.disableGuideButton && (xstate.Gamepad.wButtons & GAMEPAD_STATE_GUIDE))
		xstate.Gamepad.wButtons &= ~GAMEPAD_STATE_GUIDE;
	if (gamepad->isLockedButtons() || lockButtons) {
		if (xstate.Gamepad.wButtons & g_hosting._lockedGamepad.wButtons)
			xstate.Gamepad.wButtons &= ~g_hosting._lockedGamepad.wButtons;
	}
	gamepad->setStateSafe(xstate);
	return true;

}

bool GamepadClient::sendGamepadStateMessage(ParsecGamepadStateMessage& gamepadState, Guest& guest, int &slots, GuestPreferences prefs)
{
	return reduceUntilFirst([&](AGamepad* pad) {
		if (guest.userID == pad->owner.guest.userID)
		{
			slots++;
			if (!(isPuppetMaster && pad->isPuppet) && !pad->isLocked() && (prefs.ignoreDeviceID || gamepadState.id == pad->owner.deviceID))
			{
				if (Config::cfg.input.disableGuideButton && (gamepadState.buttons & GAMEPAD_STATE_GUIDE))
					gamepadState.buttons &= ~GAMEPAD_STATE_GUIDE;
				if (pad->isLockedButtons() || lockButtons)
				{
					if (gamepadState.buttons & g_hosting._lockedGamepad.wButtons)
						gamepadState.buttons &= ~g_hosting._lockedGamepad.wButtons;
					if (g_hosting._lockedGamepad.bLeftTrigger && gamepadState.leftTrigger != 0)
						gamepadState.leftTrigger = 0;
					if (g_hosting._lockedGamepad.bRightTrigger && gamepadState.rightTrigger != 0)
						gamepadState.rightTrigger = 0;
					if (g_hosting._lockedGamepad.sThumbLX && gamepadState.thumbLX != 0)
						gamepadState.thumbLX = 0;
					if (g_hosting._lockedGamepad.sThumbLY && gamepadState.thumbLY != 0)
						gamepadState.thumbLY = 0;
					if (g_hosting._lockedGamepad.sThumbRX && gamepadState.thumbRX != 0)
						gamepadState.thumbRX = 0;
					if (g_hosting._lockedGamepad.sThumbRY && gamepadState.thumbRY != 0)
						gamepadState.thumbRY = 0;
				}
				pad->setStateSafe(toXInput(gamepadState, pad->getState(), prefs, guest.userID));
				return true;
			}
		}
		return false;
	});
}

bool GamepadClient::sendGamepadAxisMessage(ParsecGamepadAxisMessage& gamepadAxis, Guest& guest, int& slots, GuestPreferences prefs)
{
	return reduceUntilFirst([&](AGamepad* pad) {
		if (guest.userID == pad->owner.guest.userID)
		{
			slots++;
			if (!(isPuppetMaster && pad->isPuppet) && !pad->isLocked() && (prefs.ignoreDeviceID || gamepadAxis.id == pad->owner.deviceID))
			{
				XINPUT_STATE xstate = toXInput(gamepadAxis, pad->getState(), prefs, guest.userID);
				if (pad->isLockedButtons() || lockButtons) {
					if (g_hosting._lockedGamepad.sThumbLX && gamepadAxis.axis == GAMEPAD_AXIS_LX)
						xstate.Gamepad.sThumbLX = 0;
					else if (g_hosting._lockedGamepad.sThumbLY && gamepadAxis.axis == GAMEPAD_AXIS_LY)
						xstate.Gamepad.sThumbLY = 0;
					else if (g_hosting._lockedGamepad.sThumbRX && gamepadAxis.axis == GAMEPAD_AXIS_RX)
						xstate.Gamepad.sThumbRX = 0;
					else if (g_hosting._lockedGamepad.sThumbRY && gamepadAxis.axis == GAMEPAD_AXIS_RY)
						xstate.Gamepad.sThumbRY = 0;
					else if (g_hosting._lockedGamepad.bLeftTrigger && gamepadAxis.axis == GAMEPAD_AXIS_TRIGGERL)
						xstate.Gamepad.bLeftTrigger = 0;
					else if (g_hosting._lockedGamepad.bRightTrigger && gamepadAxis.axis == GAMEPAD_AXIS_TRIGGERR)
						xstate.Gamepad.bRightTrigger = 0;
				}
				pad->setStateSafe(xstate);
				return true;
			}
		}
		return false;
	});
}

bool GamepadClient::sendGamepadButtonMessage(ParsecGamepadButtonMessage& gamepadButton, Guest& guest, int& slots, GuestPreferences prefs)
{
	return reduceUntilFirst([&](AGamepad* pad) {
		if (guest.userID == pad->owner.guest.userID)
		{
			slots++;
			if (!(isPuppetMaster && pad->isPuppet) && !pad->isLocked() && (prefs.ignoreDeviceID || gamepadButton.id == pad->owner.deviceID))
			{
				XINPUT_STATE xstate = toXInput(gamepadButton, pad->getState(), prefs, pad->owner.guest.userID);
				if (g_hosting._disableGuideButton && (xstate.Gamepad.wButtons & GAMEPAD_STATE_GUIDE))
					xstate.Gamepad.wButtons &= ~GAMEPAD_STATE_GUIDE;
				if (pad->isLockedButtons() || lockButtons) {
					if (xstate.Gamepad.wButtons & g_hosting._lockedGamepad.wButtons)
						xstate.Gamepad.wButtons &= ~g_hosting._lockedGamepad.wButtons;
				}
				pad->setStateSafe(xstate);
				return true;
			}
		}
		return false;
	});
}

/// <summary>
/// Map a button to a keyboard key for a specific profile
/// </summary>
/// <param name="profileName"></param>
/// <param name="key"></param>
/// <param name="button"></param>
/// <returns></returns>
bool GamepadClient::mapKeyboard(uint32_t userID, string button) {
	_isMapping = true;
	_mappingID = userID;
	_mappingButton = button;
	return true;
}

bool GamepadClient::sendKeyboardMessage(ParsecKeyboardMessage& keyboard, Guest& guest, int& slots, GuestPreferences prefs) {

	// Is user mapping a button and mappingName matches the current user
	if (_isMapping && _mappingID == guest.userID) {
		_isMapping = false;
		if (keyboard.pressed) {

			Keymap keymap = Keymap();
			string keyName = keymap.findKeyByValue(keyboard.code);

			_keyboardMap.mapButton(guest.name, guest.userID, _mappingButton, keyboard.code);
			g_hosting.logMessage(guest.name + " mapped " + _mappingButton + " to " + keyName);
			g_hosting.broadcastChatMessage(guest.name + " mapped " + _mappingButton + " to " + keyName);
			return false;

		}
	}

	return reduceUntilFirst([&](AGamepad* pad) {
		if (guest.userID == pad->owner.guest.userID)
		{
			slots++;
			if (!(isPuppetMaster && pad->isPuppet) && !pad->isLocked() && (prefs.ignoreDeviceID || pad->owner.isKeyboard))
			{

				XINPUT_STATE xstate = toXInput(keyboard, pad->getKeyboard(), pad->getState(), prefs, guest.userID);
				if (pad->isLockedButtons() || lockButtons) {
					if (xstate.Gamepad.wButtons & g_hosting._lockedGamepad.wButtons)
						xstate.Gamepad.wButtons &= ~g_hosting._lockedGamepad.wButtons;
					if (g_hosting._lockedGamepad.bLeftTrigger && xstate.Gamepad.bLeftTrigger != 0)
						xstate.Gamepad.bLeftTrigger = 0;
					if (g_hosting._lockedGamepad.bRightTrigger && xstate.Gamepad.bRightTrigger != 0)
						xstate.Gamepad.bRightTrigger = 0;
					if (g_hosting._lockedGamepad.sThumbLX && xstate.Gamepad.sThumbLX != 0)
						xstate.Gamepad.sThumbLX = 0;
					if (g_hosting._lockedGamepad.sThumbLY && xstate.Gamepad.sThumbLY != 0)
						xstate.Gamepad.sThumbLY = 0;
					if (g_hosting._lockedGamepad.sThumbRX && xstate.Gamepad.sThumbRX != 0)
						xstate.Gamepad.sThumbRX = 0;
					if (g_hosting._lockedGamepad.sThumbRY && xstate.Gamepad.sThumbRY != 0)
						xstate.Gamepad.sThumbRY = 0;
				}
				pad->setStateSafe(xstate);
				return true;
			}
		}
		return false;
	});
}

bool GamepadClient::tryAssignGamepad(Guest guest, uint32_t deviceID, int currentSlots, bool isKeyboard, GuestPreferences prefs)
{
	if (isSlave) {
		return false;
	}
	
	if (currentSlots >= prefs.padLimit) {
		return false;
	}
	
	int i = 0;
	return reduceUntilFirst([&](AGamepad* gamepad) {
		if (!(isPuppetMaster && gamepad->isPuppet) && (!gamepad->isLocked() && gamepad->isAttached() && !gamepad->owner.guest.isValid())) {
			if (!Config::cfg.hotseat.enabled || Hotseat::instance.checkUser(guest.userID, guest.name)) {
				if (Config::cfg.hotseat.enabled) {
					Hotseat::instance.seatUser(guest.userID, guest.name);
				}
				gamepad->setOwner(guest, deviceID, isKeyboard);
			}
			return true;
		}
		++i;
		return false;
	});
}

void GamepadClient::releaseGamepads()
{
	reduce([](AGamepad* pad) {
		pad->release();
	});

	gamepads.clear();
}

void GamepadClient::setMirror(uint32_t guestUserID, bool mirror)
{
	bool found = findPreferences(guestUserID, [&mirror](GuestPreferences& prefs) {
		prefs.mirror = mirror;
	});

	if (!found)
	{
		GuestPreferences prefs = GuestPreferences(guestUserID, 1, mirror);
		guestPreferences.push_back(prefs);
	}
}

void GamepadClient::setIgnoreDeviceID(uint32_t guestUserID, bool ignoreDeviceID)
{
	bool found = findPreferences(guestUserID, [&ignoreDeviceID](GuestPreferences& prefs) {
		prefs.ignoreDeviceID = ignoreDeviceID;
	});

	if (!found)
	{
		GuestPreferences prefs = GuestPreferences(guestUserID, 1, false, ignoreDeviceID);
		guestPreferences.push_back(prefs);
	}
}

bool GamepadClient::isRequestState(ParsecMessage message)
{
	return (message.gamepadState.buttons & (XUSB_GAMEPAD_A | XUSB_GAMEPAD_B | XUSB_GAMEPAD_X | XUSB_GAMEPAD_Y)) != 0;
}

bool GamepadClient::isRequestButton(ParsecMessage message)
{
	return message.gamepadButton.pressed && (
		message.gamepadButton.button == GAMEPAD_BUTTON_A ||
		message.gamepadButton.button == GAMEPAD_BUTTON_B ||
		message.gamepadButton.button == GAMEPAD_BUTTON_X ||
		message.gamepadButton.button == GAMEPAD_BUTTON_Y
		);
}

bool GamepadClient::isRequestKeyboard(uint32_t userID, ParsecMessage message)
{
	//-- CodeSomnia Modified Start--
	return message.keyboard.pressed && (
		_keyboardMap.isA(userID, message.keyboard.code) ||
		_keyboardMap.isB(userID, message.keyboard.code) ||
		_keyboardMap.isX(userID, message.keyboard.code) ||
		_keyboardMap.isY(userID, message.keyboard.code)
		);

	//-- CodeSomnia Modified End --
}

void GamepadClient::reduce(function<void(AGamepad*)> func)
{
	vector<AGamepad*>::iterator gi = gamepads.begin();
	for (; gi != gamepads.end(); ++gi)
	{
		func(*gi);
	}
}

bool GamepadClient::reduceUntilFirst(function<bool(AGamepad*)> func)
{
	vector<AGamepad*>::iterator gi = gamepads.begin();
	for (; gi != gamepads.end(); ++gi)
	{
		if (func(*gi))
		{
			return true;
		}
	}

	return false;
}

bool GamepadClient::findPreferences(uint32_t guestUserID, function<void(GamepadClient::GuestPreferences&)> callback)
{
	std::vector<GuestPreferences>::iterator it;
	for (it = guestPreferences.begin(); it != guestPreferences.end(); ++it)
	{
		if ((*it).userID == guestUserID)
		{
			callback(*it);
			return true;
		}
	}

	return false;
}

bool GamepadClient::isBusy()
{
	return _isBusy;
}

void GamepadClient::toggleLockGamepad(int index)
{
	if (index >= 0 && index < gamepads.size())
	{
		gamepads[index]->toggleLocked();
	}
}

void GamepadClient::toggleLockButtonsGamepad(int index)
{
	if (index >= 0 && index < gamepads.size())
	{
		gamepads[index]->toggleLockedButtons();
	}
}


XINPUT_STATE GamepadClient::toXInput(ParsecGamepadStateMessage& state, XINPUT_STATE previousState, GuestPreferences& prefs, uint32_t userID)
{
	XINPUT_STATE result = previousState;

	//if (_isAlive && _isConnected && _client != nullptr)
	result.Gamepad.wButtons = state.buttons;
	result.Gamepad.bLeftTrigger = state.leftTrigger;
	result.Gamepad.bRightTrigger = state.rightTrigger;
	result.Gamepad.sThumbLX = state.thumbLX;
	result.Gamepad.sThumbLY = state.thumbLY;
	result.Gamepad.sThumbRX = state.thumbRX;
	result.Gamepad.sThumbRY = state.thumbRY;

	if (prefs.mirror)
	{
		Bitwise::setValue(&result.Gamepad.wButtons, XUSB_GAMEPAD_DPAD_LEFT, state.thumbLX < -GAMEPAD_DEADZONE);
		Bitwise::setValue(&result.Gamepad.wButtons, XUSB_GAMEPAD_DPAD_RIGHT, state.thumbLX > GAMEPAD_DEADZONE);
		Bitwise::setValue(&result.Gamepad.wButtons, XUSB_GAMEPAD_DPAD_UP, state.thumbLY > GAMEPAD_DEADZONE);
		Bitwise::setValue(&result.Gamepad.wButtons, XUSB_GAMEPAD_DPAD_DOWN, state.thumbLY < -GAMEPAD_DEADZONE);
	}

	return result;
}

XINPUT_STATE GamepadClient::toXInput(ParsecGamepadButtonMessage& button, XINPUT_STATE previousState, GuestPreferences& prefs, uint32_t userID)
{
	XINPUT_STATE result = previousState;
	
	int buttonCode = 0;

	switch (button.button)
	{
	case GAMEPAD_BUTTON_A:
		buttonCode = XUSB_GAMEPAD_A;
		break;
	case GAMEPAD_BUTTON_B:
		buttonCode = XUSB_GAMEPAD_B;
		break;
	case GAMEPAD_BUTTON_X:
		buttonCode = XUSB_GAMEPAD_X;
		break;
	case GAMEPAD_BUTTON_Y:
		buttonCode = XUSB_GAMEPAD_Y;
		break;
	case GAMEPAD_BUTTON_BACK:
		buttonCode = XUSB_GAMEPAD_BACK;
		break;
	case GAMEPAD_BUTTON_GUIDE:
		buttonCode = XUSB_GAMEPAD_GUIDE;
		break;
	case GAMEPAD_BUTTON_START:
		buttonCode = XUSB_GAMEPAD_START;
		break;
	case GAMEPAD_BUTTON_LSTICK:
		buttonCode = XUSB_GAMEPAD_LEFT_THUMB;
		break;
	case GAMEPAD_BUTTON_RSTICK:
		buttonCode = XUSB_GAMEPAD_RIGHT_THUMB;
		break;
	case GAMEPAD_BUTTON_LSHOULDER:
		buttonCode = XUSB_GAMEPAD_LEFT_SHOULDER;
		break;
	case GAMEPAD_BUTTON_RSHOULDER:
		buttonCode = XUSB_GAMEPAD_RIGHT_SHOULDER;
		break;
	case GAMEPAD_BUTTON_DPAD_UP:
		buttonCode = XUSB_GAMEPAD_DPAD_UP;
		break;
	case GAMEPAD_BUTTON_DPAD_DOWN:
		buttonCode = XUSB_GAMEPAD_DPAD_DOWN;
		break;
	case GAMEPAD_BUTTON_DPAD_LEFT:
		buttonCode = XUSB_GAMEPAD_DPAD_LEFT;
		break;
	case GAMEPAD_BUTTON_DPAD_RIGHT:
		buttonCode = XUSB_GAMEPAD_DPAD_RIGHT;
		break;
	default:
		break;
	}

	Bitwise::setValue(&result.Gamepad.wButtons, buttonCode, button.pressed);

	return result;
}

XINPUT_STATE GamepadClient::toXInput(ParsecGamepadAxisMessage& axis, XINPUT_STATE previousState, GuestPreferences& prefs, uint32_t userID)
{
	XINPUT_STATE result = previousState;

	switch (axis.axis)
	{
	case GAMEPAD_AXIS_LX:
		result.Gamepad.sThumbLX = axis.value;
		if (prefs.mirror)
		{
			Bitwise::setValue(&result.Gamepad.wButtons, XUSB_GAMEPAD_DPAD_LEFT, axis.value < -GAMEPAD_DEADZONE);
			Bitwise::setValue(&result.Gamepad.wButtons, XUSB_GAMEPAD_DPAD_RIGHT, axis.value > GAMEPAD_DEADZONE);
		}
		break;
	case GAMEPAD_AXIS_LY:
		result.Gamepad.sThumbLY = -axis.value;
		if (prefs.mirror)
		{
			Bitwise::setValue(&result.Gamepad.wButtons, XUSB_GAMEPAD_DPAD_UP, axis.value < -GAMEPAD_DEADZONE);
			Bitwise::setValue(&result.Gamepad.wButtons, XUSB_GAMEPAD_DPAD_DOWN, axis.value > GAMEPAD_DEADZONE);
		}
		break;
	case GAMEPAD_AXIS_RX:
		result.Gamepad.sThumbRX = axis.value;
		break;
	case GAMEPAD_AXIS_RY:
		result.Gamepad.sThumbRY = -axis.value;
		break;
	case GAMEPAD_AXIS_TRIGGERL:
		result.Gamepad.bLeftTrigger = axis.value;
		break;
	case GAMEPAD_AXIS_TRIGGERR:
		result.Gamepad.bRightTrigger = axis.value;
		break;
	default:
		break;
	}

	return result;
}

XINPUT_STATE GamepadClient::toXInput(ParsecKeyboardMessage& key, AGamepad::Keyboard& keyboard, XINPUT_STATE previousState, GuestPreferences& prefs, uint32_t userID)
{
	//-- CodeSomnia Modified Start--
	XINPUT_STATE result = previousState;

	// Left Stick
	if (_keyboardMap.isLLeft(userID, key.code))
	{
		if (key.pressed) result.Gamepad.sThumbLX = GAMEPAD_SHORT_MIN;
		else result.Gamepad.sThumbLX = keyboard.LRight ? GAMEPAD_SHORT_MAX : 0;
		if (prefs.mirror) Bitwise::setValue(&result.Gamepad.wButtons, XUSB_GAMEPAD_DPAD_LEFT, key.pressed);
		keyboard.LLeft = key.pressed;
	}

	if (_keyboardMap.isLRight(userID, key.code))
	{
		if (key.pressed) result.Gamepad.sThumbLX = GAMEPAD_SHORT_MAX;
		else result.Gamepad.sThumbLX = keyboard.LLeft ? GAMEPAD_SHORT_MIN : 0;
		if (prefs.mirror) Bitwise::setValue(&result.Gamepad.wButtons, XUSB_GAMEPAD_DPAD_RIGHT, key.pressed);
		keyboard.LRight = key.pressed;
	}

	if (_keyboardMap.isLUp(userID, key.code))
	{
		if (key.pressed) result.Gamepad.sThumbLY = GAMEPAD_SHORT_MAX;
		else result.Gamepad.sThumbLY = keyboard.LDown ? GAMEPAD_SHORT_MIN : 0;
		if (prefs.mirror) Bitwise::setValue(&result.Gamepad.wButtons, XUSB_GAMEPAD_DPAD_UP, key.pressed);
		keyboard.LUp = key.pressed;
	}

	if (_keyboardMap.isLDown(userID, key.code))
	{
		if (key.pressed) result.Gamepad.sThumbLY = GAMEPAD_SHORT_MIN;
		else result.Gamepad.sThumbLY = keyboard.LUp ? GAMEPAD_SHORT_MAX : 0;
		if (prefs.mirror) Bitwise::setValue(&result.Gamepad.wButtons, XUSB_GAMEPAD_DPAD_DOWN, key.pressed);
		keyboard.LDown = key.pressed;
	}

	// Right Stick
	if (_keyboardMap.isRLeft(userID, key.code))
	{
		if (key.pressed) result.Gamepad.sThumbRX = GAMEPAD_SHORT_MIN;
		else result.Gamepad.sThumbRX = keyboard.RRight ? GAMEPAD_SHORT_MAX : 0;
		keyboard.RLeft = key.pressed;
	}

	if (_keyboardMap.isRRight(userID, key.code))
	{
		if (key.pressed) result.Gamepad.sThumbRX = GAMEPAD_SHORT_MAX;
		else result.Gamepad.sThumbRX = keyboard.RLeft ? GAMEPAD_SHORT_MIN : 0;
		keyboard.RRight = key.pressed;
	}

	if (_keyboardMap.isRUp(userID, key.code))
	{
		if (key.pressed) result.Gamepad.sThumbRY = GAMEPAD_SHORT_MAX;
		else result.Gamepad.sThumbRY = keyboard.RDown ? GAMEPAD_SHORT_MIN : 0;
		keyboard.RUp = key.pressed;
	}

	if (_keyboardMap.isRDown(userID, key.code))
	{
		if (key.pressed) result.Gamepad.sThumbRY = GAMEPAD_SHORT_MIN;
		else result.Gamepad.sThumbRY = keyboard.RUp ? GAMEPAD_SHORT_MAX : 0;
		keyboard.RDown = key.pressed;
	}

	// DPad
	if (_keyboardMap.isDLeft(userID, key.code))  Bitwise::setValue(&result.Gamepad.wButtons, XUSB_GAMEPAD_DPAD_LEFT, key.pressed);
	if (_keyboardMap.isDRight(userID, key.code)) Bitwise::setValue(&result.Gamepad.wButtons, XUSB_GAMEPAD_DPAD_RIGHT, key.pressed);
	if (_keyboardMap.isDUp(userID, key.code))    Bitwise::setValue(&result.Gamepad.wButtons, XUSB_GAMEPAD_DPAD_UP, key.pressed);
	if (_keyboardMap.isDDown(userID, key.code))  Bitwise::setValue(&result.Gamepad.wButtons, XUSB_GAMEPAD_DPAD_DOWN, key.pressed);

	// Face buttons
	if (_keyboardMap.isA(userID, key.code)) Bitwise::setValue(&result.Gamepad.wButtons, XUSB_GAMEPAD_A, key.pressed);
	if (_keyboardMap.isB(userID, key.code)) Bitwise::setValue(&result.Gamepad.wButtons, XUSB_GAMEPAD_B, key.pressed);
	if (_keyboardMap.isX(userID, key.code)) Bitwise::setValue(&result.Gamepad.wButtons, XUSB_GAMEPAD_X, key.pressed);
	if (_keyboardMap.isY(userID, key.code)) Bitwise::setValue(&result.Gamepad.wButtons, XUSB_GAMEPAD_Y, key.pressed);

	// Center
	if (_keyboardMap.isBack(userID, key.code))  Bitwise::setValue(&result.Gamepad.wButtons, XUSB_GAMEPAD_BACK, key.pressed);
	if (_keyboardMap.isStart(userID, key.code)) Bitwise::setValue(&result.Gamepad.wButtons, XUSB_GAMEPAD_START, key.pressed);

	// Shoulders
	if (_keyboardMap.isLB(userID, key.code)) Bitwise::setValue(&result.Gamepad.wButtons, XUSB_GAMEPAD_LEFT_SHOULDER, key.pressed);
	if (_keyboardMap.isRB(userID, key.code)) Bitwise::setValue(&result.Gamepad.wButtons, XUSB_GAMEPAD_RIGHT_SHOULDER, key.pressed);

	// Triggers
	if (_keyboardMap.isLT(userID, key.code)) result.Gamepad.bLeftTrigger = (key.pressed ? GAMEPAD_SHORT_MAX : GAMEPAD_SHORT_MIN);
	if (_keyboardMap.isRT(userID, key.code)) result.Gamepad.bRightTrigger = (key.pressed ? GAMEPAD_SHORT_MAX : GAMEPAD_SHORT_MIN);

	// Thumbs
	if (_keyboardMap.isLThumb(userID, key.code)) Bitwise::setValue(&result.Gamepad.wButtons, XUSB_GAMEPAD_LEFT_THUMB, key.pressed);
	if (_keyboardMap.isRThumb(userID, key.code)) Bitwise::setValue(&result.Gamepad.wButtons, XUSB_GAMEPAD_RIGHT_THUMB, key.pressed);

	//-- CodeSomnia Modified End--
	return result;
}


