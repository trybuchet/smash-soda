#pragma once

#include <sstream>
#include "ACommand.h"
#include "../../../core/GamepadClient.h"

class CommandRC : public ACommand
{
public:

	/**
	 * @brief Construct a new CommandDC object
	 *
	 * @param msg
	 * @param gamepadClient
	 */
	CommandRC(const char* msg, Guest& sender, GamepadClient& gamepadClient)
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
			setReply("Usage: !rc <integer in range [1, " + std::to_string(nGamepads) + "]>\nExample: !rc 4\0");
			return false;
		}

		int slot = 1;
		try {
			slot = std::stoi(getArgs()[0]);
		}
		catch (std::invalid_argument) {
			setReply("Usage: !rc <integer in range [1, " + std::to_string(nGamepads) + "]>\nExample: !rc 4\0");
			return false;
		}

		// In range [1, nGamepads]
		if (slot < 1 || slot > nGamepads) {
			setReply("Usage: !rc <integer in range [1, " + std::to_string(nGamepads) + "]>\nExample: !rc 4\0");
			return false;
		}

		_gamepadClient.connect(slot - 1);
		setReply("Reconnected gamepad " + std::to_string(slot) + ".\0");

		return true;
	}

	/**
	 * @brief Get the prefixes object
	 *
	 * @return std::vector<const char*>
	 */
	static vector<const char*> prefixes() {
		return vector<const char*> { "!rc" };
	}

private:
	static vector<const char*> internalPrefixes() {
		return vector<const char*> { "!rc " };
	}

	GamepadClient& _gamepadClient;

};


