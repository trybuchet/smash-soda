#pragma once

#include "Cache.h"
#include "ACommand.h"
#include "../../../core/GamepadClient.h"
#include "Macro.h"

class CommandBB : public ACommand
{
public:

	/**
	 * Example of how to use the command
	 */
	std::string usage = "Usage: !bb";

	/**
	 * @brief Construct a new CommandBB object
	 * 
	 * @param gamepadClient 
	 * @param macro 
	 * @param sender 
	 */
	CommandBB(const char* msg, Guest& sender, GamepadClient& gamepadClient, Macro& macro, bool isHost = false)
		: ACommand(msg, sender), _gamepadClient(gamepadClient), _macro(macro), _sender(sender), _isHost(isHost)
	{}

	/**
	 * @brief Run the command
	 * @return true if the command was successful
	 */
	bool run() override {

		if (hasPermission()) {
			_macro.pressButtonForAll(ParsecGamepadButton::GAMEPAD_BUTTON_B);
			_macro.pressButtonForAll(ParsecGamepadButton::GAMEPAD_BUTTON_B);
			setReply("Everybody's B button was pressed twice!\0");
		}
		else {
			setReply("Only mods and VIPs can use this command!\0");
		}
		
		return true;
	}

	/**
	 * @brief Get the prefixes object
	 * @return vector<const char*> 
	 */
	static vector<const char*> prefixes() {
		return vector<const char*> { "!bb", "!bbb" };
	}

protected:
	GamepadClient& _gamepadClient;
	Macro& _macro;
	Guest& _sender;
	bool _isHost = false;

	bool hasPermission() {
		if (_isHost) {
			return true;
		}
		Tier tier = Cache::cache.tierList.getTier(_sender.userID);
		if (tier == Tier::MOD || tier == Tier::GOD ||
			Cache::cache.vipList.isVIP(_sender.userID)) {
			return true;
		} else {
			return false;
		}
	}
};


