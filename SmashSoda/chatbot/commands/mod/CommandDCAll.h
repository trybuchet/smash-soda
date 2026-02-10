#pragma once

#include "ACommand.h"
#include "../../../core/GamepadClient.h"

class CommandDCAll : public ACommand
{
public:

	/**
	 * @brief Construct a new CommandLockAll object
	 *
	 * @param gamepadClient
	 */
	CommandDCAll(const char* msg, Guest& sender, GamepadClient& gamepadClient)
		: ACommand(msg, sender), _gamepadClient(gamepadClient)
	{}

	/**
	 * @brief Run the command
	 *
	 * @return true
	 * @return false
	 */
	bool run() override {

		std::vector<AGamepad*>::iterator gi = _gamepadClient.gamepads.begin();
		for (; gi != _gamepadClient.gamepads.end(); ++gi) {
			if ((*gi)->isConnected()) {
				(*gi)->disconnect();
			}
			else {
				(*gi)->connect();
			}
		}
		setReply("All gamepads disconnected");

		return true;

	}

	static vector<const char*> prefixes() {
		return vector<const char*> { "!dcall" };
	}

protected:
	GamepadClient& _gamepadClient;
};


