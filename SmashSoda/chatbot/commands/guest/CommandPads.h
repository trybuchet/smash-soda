#pragma once

#include "ACommand.h"
#include "../../../core/GamepadClient.h"
#include <sstream>

class CommandPads : public ACommand
{
public:

	/**
	 * @brief Construct a new CommandPads object
	 * 
	 * @param gamepadClient
	 */
	CommandPads(const char* msg, Guest& sender, GamepadClient &gamepadClient)
		: ACommand(msg, sender), _gamepadClient(gamepadClient)
	{}

	/**
	 * @brief Run the command
	 * 
	 * @return true
	 * @return false
	 */
	bool run() override {
		std::ostringstream reply;
		reply << Config::cfg.chatbotName + "Gamepad Holders:\n";

		std::vector<AGamepad*>::iterator gi = _gamepadClient.gamepads.begin();
		uint16_t i = 1;
		int onlineCount = 0;
		for (; gi != _gamepadClient.gamepads.end(); ++gi)
		{
			if ((*gi)->isConnected())
			{
				onlineCount++;

				reply << "\t\t[" << i << "] \t";

				if (_gamepadClient.isPuppetMaster && (*gi)->isPuppet)
				{
					reply << "(Host) Puppet Master" << "\n";
				}
				else if (!(*gi)->owner.guest.isValid())
				{
					reply << "\n";
				}
				else
				{
					reply << "(" << (*gi)->owner.guest.userID << ")\t" << (*gi)->owner.guest.name << "\n";
				}
			}

			++i;
		}

		if (onlineCount <= 0) {
			reply << "\t\tNo gamepads connected.\n";
		}

		reply << "\0";

		_replyMessage = reply.str();
		isBotCommand = true;
		return true;
	}

	/**
	 * @brief Get the prefixes object
	 * 
	 * @return std::vector<const char*>
	 */
	static vector<const char*> prefixes() {
		return vector<const char*> { "!pads", "!pad " };
	}

protected:
	GamepadClient& _gamepadClient;
};




