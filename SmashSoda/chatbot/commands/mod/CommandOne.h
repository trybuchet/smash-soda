#pragma once

#include "ACommand.h"
#include "../../../core/GamepadClient.h"

class CommandOne : public ACommand
{
public:

	/**
	 * @brief Construct a new CommandOne object
	 * 
	 * @param sender
	 * @param gamepadClient
	 */
	CommandOne(const char* msg, Guest& sender, GamepadClient &gamepadClient)
		: ACommand(msg, sender), _gamepadClient(gamepadClient)
	{}

	/**
	 * @brief Run the command
	 * 
	 * @return true
	 * @return false
	 */
	bool run() override {
		bool isOne = _gamepadClient.toggleIgnoreDeviceID(_sender.userID);
		setReply(_sender.name + " toggled ignore device ID mode:\t" + (isOne ? "ON" : "OFF"));
		return true;
	}

	/**
	 * @brief Get the prefixes object
	 * 
	 * @return std::vector<const char*>
	 */
	static vector<const char*> prefixes() {
		return vector<const char*> { "!one" };
	}

protected:
	GamepadClient& _gamepadClient;
};



