#pragma once

#include <iostream>
#include "ACommand.h"
#include "../../../core/GamepadClient.h"

class CommandAddXbox : public ACommand
{
public:

	CommandAddXbox(const char* msg, Guest& sender, GamepadClient& gamepadClient)
		: ACommand(msg, sender), _gamepadClient(gamepadClient)
	{}

	bool run() override {

		_gamepadClient.createGamepad(AGamepad::Type::XBOX, true);
		setReply("Added xbox controller.\0");
		return true;
	}

	static vector<const char*> prefixes() {
		return vector<const char*> { "!addxbox", "!addxboxpad" };
	}

protected:
	GamepadClient& _gamepadClient;
};


