#pragma once

#include "ACommand.h"
#include "../../../core/GamepadClient.h"

class CommandLock : public ACommand
{
public:

	/**
	 * @brief Construct a new CommandLock object
	 * 
	 * @param msg
	 * @param sender
	 * @param gamepadClient
	 */
	CommandLock(const char* msg, Guest& sender, GamepadClient& gamepadClient)
		: ACommand(msg, sender), _gamepadClient(gamepadClient)
	{}

	/**
	 * @brief Run the command
	 * 
	 * @return true
	 * @return false
	 */
	bool run() override {

		size_t nGamepads = _gamepadClient.gamepads.size();

		if (nGamepads == 0) {
			setReply("No gamepads available.\0");
			return false;
		}

		if (getArgs().size() == 0) {
			setReply("Usage: !swap <integer in range [1, " + std::to_string(nGamepads) + "]>\nExample: !swap 4\0");
			return false;
		}

		int slot = 1;
		try {
			slot = std::stoi(getArgs()[0]);
		}
		catch (std::invalid_argument) {
			setReply("Usage: !swap <integer in range [1, " + std::to_string(nGamepads) + "]>\nExample: !swap 4\0");
			return false;
		}

		// In range [1, nGamepads]
		if (slot < 1 || slot > nGamepads) {
			setReply("Usage: !swap <integer in range [1, " + std::to_string(nGamepads) + "]>\nExample: !swap 4\0");
			return false;
		}

		AGamepad* pad = _gamepadClient.getGamepad(slot-1);
		if (pad) {

			if (pad->isLocked()) {
				setReply("Gamepad " + std::to_string(slot) + " locked!\0");
			}
			else {
				setReply("Gamepad " + std::to_string(slot) + " unlocked!\0");
			}
			pad->setLocked(!pad->isLocked());
		}
		return true;
	}

	/**
	 * @brief Get the prefixes object
	 * 
	 * @return std::vector<const char*>
	 */
	static vector<const char*> prefixes() {
		return vector<const char*> { "!lock" };
	}

protected:
	static vector<const char*> internalPrefixes()
	{
		return vector<const char*> { "!lock " };
	}
	GamepadClient& _gamepadClient;
};


