
#pragma once

#include "../core/GuestList.h"
#include "../Core/Cache.h"
#include "../Core/Config.h"
#include "../Tier.h"
#include "Stringer.h"

class ACommand
{
public:

	/**
	 * Example of how to use the command
	 */
	std::string usage = "";

	/**
	* Is this command a bot command. Used
	* to exit out of ChatBot run execution if
	* the command is not a bot command.
	*/
	bool isBotCommand = true;

	/**
	 * Run the command
	 */
	virtual bool run() = 0;

	/**
	 * Constructor
	 * 
	 * @param _msg The message
	 * @param sender The sender of the message
	 */
	ACommand(const char* _msg, Guest& sender) : _msg(_msg), _sender(sender) {
	}

	/**
	* Get the entire message
	*/
	const char* getMsg() const { return _msg; }

	/**
	* Get the chat command
	*/
	std::string getCommand() {
		std::vector<std::string> tokens = split(_msg, ' ');
		return tokens[1];
	}

	/**
	* Get the arguments of a message
	*/
	vector<string> getArgs() {
		vector<string> tokens = split(_msg, ' ');
		tokens.erase(tokens.begin());
		return tokens;
	}

	/**
	* Get only the arguments, minus the command
	*/
	string getArgString() {
		std::string str = _msg;
		size_t pos = str.find(" ");
		if (pos != std::string::npos) {
			str = str.substr(pos + 1);
		} else {
			str = "";
		}

		return str;
	}

	/**
	* Returns usage example if command is invalid
	*/
	bool error() {
		setReply(usage);
		return false;
	}

	/**
	* Get the sender of the command
	*/
	Guest& getSender() {
		return _sender;
	}

	/**
	 * Get the reply message
	 * @return The reply message
	 */
	const std::string getReply() const { 
		return _replyMessage; 
	}

	/**
	 * Prefixes the reply with the Chatbot's name
	 *
	 * @param message The message to send
	 */
	void setReply(const std::string& message) {
		_replyMessage = Config::cfg.chatbotName + message;
		isBotCommand = true;
	}

	bool fuzzyMatch(std::string str, std::string target) {

		static const uint64_t MINIMUM_MATCH = 3;
		uint64_t closestDistance = STRINGER_MAX_DISTANCE;
		uint64_t distance = STRINGER_MAX_DISTANCE;
		bool found = false;

		if (str.length() < MINIMUM_MATCH) {
			return false;
		}

		distance = Stringer::fuzzyDistance(str, target);
		if (distance <= closestDistance && distance <= STRINGER_DISTANCE_CHARS(MINIMUM_MATCH)) {
			closestDistance = distance;
			found = true;
		}

		return found;

	}

protected:
	const char* _msg;
	Guest& _sender;
	std::string _replyMessage = "";

	/**
	 * Split a string by a delimiter
	 *
	 * @param str The string to split
	 * @param delimiter The delimiter to split by
	 */
	std::vector<std::string> split(const std::string& str, char delimiter) {
		std::vector<std::string> tokens;
		std::stringstream ss(str);
		std::string token;

		while (std::getline(ss, token, delimiter)) {
			tokens.push_back(token);
		}

		return tokens;
	}
};


