#pragma once

#include <iostream>
#include "ACommand.h"
#include "../../../core/GamepadClient.h"

class CommandAddPS : public ACommand
{
public:

	/**
	 * @brief Construct a new CommandAddPS object
	 * 
	 * @param gamepadClient The gamepad client
	 */
	CommandAddPS(const char* msg, Guest& sender, GamepadClient& gamepadClient)
		: ACommand(msg, sender), _gamepadClient(gamepadClient)
	{}

	/**
	 * @brief Run the command
	 * @return true if the command was successful
	 */
	bool run() override {
		_gamepadClient.createGamepad(AGamepad::Type::DUALSHOCK, true);
		setReply("Added playstation controller.\0");
		return true;
	}

	/**
	 * @brief Get the prefixes
	 * @return The prefixes
	 */
	static vector<const char*> prefixes() {
		return vector<const char*> { "!addps", "!addds", "!addpspad", "!adddspad" };
	}

protected:
	GamepadClient& _gamepadClient;
};


