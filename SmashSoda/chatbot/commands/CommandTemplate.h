#pragma once

#include "Config.h"
#include "ACommand.h"

using namespace std;

class CommandTemplate : public ACommand {
public:

	/*
	This is the constructor where you can pass in any
	arguments and initialize any variables.

	The base class can inherit from ACommand, ACommandSearchUser,
	ACommandIntegerArg and ACommandStringArg.

	-- EXAMPLES ----------------

	// ACommand
	#include "../ACommand.h"

	class CommandTemplate : public ACommand {
		public:
			CommandTemplate(const char* msg) : ACommand(msg) { }
	}

	// ACommandSearchUser
	#include "ACommandSearchUser.h"

	class CommandTemplate : public ACommandSearchUser {
		public:
			CommandTemplate(const char* msg, GuestList& guests)
			: ACommandSearchUser(msg, internalPrefixes(), guests)
			{}
	}

	// ACommandIntegerArg
	#include "ACommandIntegerArg.h"

	class CommandTemplate : public ACommandIntegerArg {
		public:
			CommandTemplate(const char* msg)
			: ACommandIntegerArg(msg, internalPrefixes())
			{}
	}

	// ACommandStringArg
	#include "ACommandStringArg.h"

	class CommandTemplate : public ACommandStringArg {
		public:
			CommandTemplate(const char* msg)
			: ACommandStringArg(msg, internalPrefixes())
			{}
	}
	----------------------------
	*/
	CommandTemplate(const char* msg, Guest& sender) :
	ACommand(msg, sender) {}

	/**
	 * @brief Run the command
	 * This is the main function that will be called when the command is run.
	 * 
	 * @return true
	 */
	bool run() override {

		// You can set the response using the private variable _replyMessage.
		_replyMessage = "This is an example command!";

		/*
		When using a string or integer argument, you must first run the parent
		class's run command, then a private variable will be set with the value

		-- EXAMPLES ----------------
		// ACommandIntegerArg
		// The argument will be stored in _intArg
		if ( ACommandStringArg::run() ) {
			_replyMessage = "You entered: " + to_string(_intArg);
		}

		// ACommandStringArg
		// The argument will be stored in _stringArg
		if ( ACommandStringArg::run() ) {
			_replyMessage = "You entered: " + _stringArg;
		}
		----------------------------
		*/

		/*
		When searching for a user, you can handle the result of the search
		like so:

		-- EXAMPLE ----------------

		ACommandSearchUser::run();
		switch (_searchResult) {
			case SEARCH_USER_RESULT::NOT_FOUND:
				_replyMessage = Config::cfg.chat.chatbotName + " | User not found.\0";
			break;

			case SEARCH_USER_RESULT::FOUND:
				_replyMessage = "User found!";
			break;

			case SEARCH_USER_RESULT::FAILED:
			default:
				_replyMessage = Config::cfg.chat.chatbotName + " | Usage: !example MickeyUK \0";
				break;
		}
		----------------------------
		*/

		// Can just leave this true unless you want to handle errors in some way.
		return true;
		
	}

	/**
	 * @brief Get the prefixes object
	 * These are the prefixes that will trigger the command when a user types in chat.
	 * 
	 * @return vector<const char*>
	 */
	static vector<const char*> prefixes() {
		return vector<const char*> { "!example" };
	}

protected:

	/*
	Create private variables here for arguments passed in the constructor.

	-- EXAMPLE ----------------
	CommandTemplate(const char* msg, Guest& sender, GuestList& guests, GamepadClient& gamepadClient)
		: ACommandSearchUser(msg, internalPrefixes(), guests), _sender(sender), _gamepadClient(gamepadClient)
	{}

	protected:
		Guest& _sender;
		GamepadClient& _gamepadClient;
	*/

	/**
	 * @brief Get the internal prefixes object
	 * Used to set the prefixes for the command when using a string or integer argument.
	 * 
	 * @return vector<const char*>
	 */
	static vector<const char*> internalPrefixes() {
		return vector<const char*> { "!example " };
	}
};


