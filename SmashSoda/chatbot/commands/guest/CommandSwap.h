#pragma once

#include "ACommand.h"
#include "../../../core/GamepadClient.h"

class CommandSwap : public ACommand
{
public:
	
	/**
	 * @brief Construct a new CommandSwap object
	 * 
	 * @param msg 
	 * @param sender 
	 * @param gamepadClient 
	 */
	CommandSwap(const char* msg, Guest &sender, GamepadClient  &gamepadClient)
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

		GamepadClient::PICK_REQUEST result = _gamepadClient.pick(_sender, slot - 1);
		
		bool rv = false;
		std::ostringstream reply;

		switch (result)
		{
		case GamepadClient::PICK_REQUEST::OK:
			reply
				<< Config::cfg.chatbotName + "Gamepad " << slot << " was given to " << _sender.name << "\t(#" << _sender.userID << ")\n"
				<< "\t\tType !pads to see the gamepad list.\0";
			rv = true;
			break;
		case GamepadClient::PICK_REQUEST::DISCONNECTED:
			reply
				<< Config::cfg.chatbotName << _sender.name << ", gamepad " << slot << " is offline.\n"
				<< "\t\tType !pads to see the gamepad list.\0";
			break;
		case GamepadClient::PICK_REQUEST::SAME_USER:
			reply
				<< Config::cfg.chatbotName << _sender.name << ", you have that gamepad already.\n"
				<< "\t\tType !pads to see the gamepad list.\0";
			break;
		case GamepadClient::PICK_REQUEST::TAKEN:
			reply
				<< Config::cfg.chatbotName << _sender.name << ", the gamepad you tried to pick is already taken.\n"
				<< "\t\tType !pads to see the gamepad list.\0";
			break;
		case GamepadClient::PICK_REQUEST::EMPTY_HANDS:
			reply
				<< Config::cfg.chatbotName << _sender.name << ", you must be holding a gamepad to use !swap command.\n"
				<< "\t\tPress any face button (A, B, X, Y) to receive a random gamepad (if available).\n"
				<< "\t\tType !pads to see the gamepad list.\0";
			break;
		case GamepadClient::PICK_REQUEST::LIMIT_BLOCK:
			reply
				<< Config::cfg.chatbotName << _sender.name << ", your current gamepad limit is set to 0.\0";
			break;
		case GamepadClient::PICK_REQUEST::OUT_OF_RANGE:
			reply
				<< Config::cfg.chatbotName << _sender.name << ", your gamepad index is wrong (valid range is [1, 4]).\n"
				<< "\t\tType !pads to see the gamepad list.\0";
			break;
		case GamepadClient::PICK_REQUEST::PUPPET:
			reply
				<< Config::cfg.chatbotName << _sender.name << ", puppet master is handling that gamepad.\n"
				<< "\t\tType !pads to see the gamepad list.\0";
			break;
		default:
			break;
		}

		_replyMessage = reply.str();
		return false;
	}

	/**
	 * @brief Get the prefixes object
	 * 
	 * @return vector<const char*> 
	 */
	static vector<const char*> prefixes() {
		return vector<const char*> { "!swap", "!pick", "!slot" };
	}

protected:
	static vector<const char*> internalPrefixes()
	{
		return vector<const char*> { "!swap ", "!pick ", "!slot " };
	}
	string _msg;
	GamepadClient& _gamepadClient;
};




