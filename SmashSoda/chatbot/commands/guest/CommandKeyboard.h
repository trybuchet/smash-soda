#pragma once

#include "parsec-dso.h"
#include "ACommand.h"
#include "../../../core/GamepadClient.h"

class CommandKeyboard : public ACommand
{
public:

	/**
	 * Example of how to use the command
	 */
	std::string usage = "See instructions at https ://github.com/soda-arcade/smash-soda/wiki/keyboard";

	/**
	 * @brief Construct a new CommandKeyboard object
	 * 
	 * @param msg 
	 * @param sender 
	 * @param gamepadClient 
	 */
	CommandKeyboard(const char* msg, Guest& sender, GamepadClient& gamepadClient)
		: ACommand(msg, sender), _gamepadClient(gamepadClient)
	{}

	/**
	 * @brief Run the command
	 * @return true if the command was successful
	 */
	bool run() override {

		// Valid button check
		/*if (!ACommandStringArg::run() || _gamepadClient.getKeyMap().isValidButtonName(toLower(_stringArg)) == false) {
			setReply("See instructions at https://github.com/mickeyuk/smash-soda/wiki/keyboard");
			return false;
		}*/

		if (getArgs().size() < 1) {
			setReply(usage);
			return false;
		}

		string btn = getArgs()[0];
		if (_gamepadClient.getKeyMap().isValidButtonName(toLower(btn)) == false) {
			setReply(usage);
			return false;
		}

		_gamepadClient.mapKeyboard(_sender.userID, toLower(btn));
		setReply(_sender.name + ", exit the chat, double click the Parsec window and press a key to map to " + toUpper(btn) + ".\0");

		return true;

	}

	/**
	 * @brief Get the prefixes object
	 * @return vector<const char*> 
	 */
	static vector<const char*> prefixes() {
		return vector<const char*> { "!kb" };
	}

protected:
	GamepadClient& _gamepadClient;

	// Convert string to lowercase
	string toLower(string str) {
		transform(str.begin(), str.end(), str.begin(), ::tolower);
		return str;
	}

	// Convert string to uppercase
	string toUpper(string str) {
		transform(str.begin(), str.end(), str.begin(), ::toupper);
		return str;
	}

	static vector<const char*> internalPrefixes() {
		return vector<const char*> { "!kb " };
	}
};


