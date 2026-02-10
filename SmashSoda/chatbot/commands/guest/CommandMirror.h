#pragma once

#include "ACommand.h"
#include "../../../core/GamepadClient.h"

class CommandMirror : public ACommand
{
public:

	/**
	 * @brief Construct a new CommandMirror object
	 * 
	 * @param sender 
	 * @param gamepadClient 
	 */
	CommandMirror(const char* msg, Guest& sender, GamepadClient &gamepadClient)
		: ACommand(msg, sender), _gamepadClient(gamepadClient)
	{}

	/**
	 * @brief Run the command
	 * @return true if the command was successful
	 */
	bool run() override {
		bool isMirrored = _gamepadClient.toggleMirror(_sender.userID);
		setReply(_sender.name + " toggled mirror mode:\t" + (isMirrored ? "ON" : "OFF") + "\0");
		return true;
	}

	/**
	 * @brief Get the prefixes object
	 * @return vector<const char*> 
	 */
	static vector<const char*> prefixes() {
		return vector<const char*> { "!mirror" };
	}

protected:
	GamepadClient& _gamepadClient;
};



