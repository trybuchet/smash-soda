#pragma once

#include "ACommand.h"

using namespace std;

class CommandDiscord : public ACommand
{
public:

	/**
	 * Example of how to use the command
	 */
	std::string usage = "Usage: !discord";

	/**
	 * @brief Construct a new CommandDiscord object
	 * 
	 * @param sender 
	 */
	CommandDiscord(const char* msg, Guest& sender)
		: ACommand(msg, sender)
	{}

	/**
	 * @brief Run the command
	 * @return true if the command was successful
	 */
	bool run() override {
		
		if (Config::cfg.chat.discord != "") {
			_replyMessage = Config::cfg.chat.discord.c_str();
			isBotCommand = true;
		}

		return true;
	}

	/**
	 * @brief Get the prefixes object
	 * @return vector<const char*> 
	 */
	static vector<const char*> prefixes() {
		return vector<const char*> { "!discord" };
	}

protected:
};




