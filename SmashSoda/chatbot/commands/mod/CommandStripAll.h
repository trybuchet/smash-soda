#pragma once

#include "../../../Core/Config.h"
#include "ACommand.h"
#include "../../../core/GamepadClient.h"

using namespace std;

class CommandStripAll : public ACommand {
public:

	/**
	 * @brief Construct a new CommandStripAll object
	 * 
	 * @param msg
	 * @param gamepadClient
	 */
	CommandStripAll(const char* msg, Guest& sender, GamepadClient& gamepadClient)
		: ACommand(msg, sender), _gamepadClient(gamepadClient) {}

	/**
	 * @brief Run the command
	 * 
	 * @return true
	 * @return false
	 */
	bool run() override {

		_gamepadClient.stripAll();
		_replyMessage = Config::cfg.chatbotName + "Stripped all pads.";

		return true;

	}

	/**
	 * @brief Get the prefixes object
	 * 
	 * @return vector<const char*>
	 */
	static vector<const char*> prefixes() {
		return vector<const char*> { "!yoink" };
	}

protected:

	GamepadClient& _gamepadClient;

	static vector<const char*> internalPrefixes() {
		return vector<const char*> { "!yoink " };
	}
};


