#pragma once

#include "ACommand.h"
#include "../../../core/GamepadClient.h"
#include "Macro.h"

class CommandLockAll : public ACommand
{
public:

	/**
	 * @brief Construct a new CommandLockAll object
	 * 
	 * @param gamepadClient
	 */
	CommandLockAll(const char* msg, Guest& sender, GamepadClient& gamepadClient)
		: ACommand(msg, sender), _gamepadClient(gamepadClient)
	{}

	/**
	 * @brief Run the command
	 * 
	 * @return true
	 * @return false
	 */
	bool run() override {

		if (Config::cfg.input.lockedPads) {

			std::vector<AGamepad*>::iterator gi = _gamepadClient.gamepads.begin();
			for (; gi != _gamepadClient.gamepads.end(); ++gi) {
				(*gi)->setLocked(false);
			}
			Config::cfg.input.lockedPads = false;
			_replyMessage = Config::cfg.chatbotName + "Everybody's pads were unlocked!\0";

		}
		else {

			std::vector<AGamepad*>::iterator gi = _gamepadClient.gamepads.begin();
			for (; gi != _gamepadClient.gamepads.end(); ++gi) {
				(*gi)->setLocked(true);
			}
			Config::cfg.input.lockedPads = true;
			_replyMessage = Config::cfg.chatbotName + "Everybody's pads were locked!\0";

		}

		return true;

	}

	static vector<const char*> prefixes() {
		return vector<const char*> { "!lockall", "!unlockall", "!lockpads", "!unlockpads" };
	}

protected:
	GamepadClient& _gamepadClient;
};


