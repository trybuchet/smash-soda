#pragma once

#include "Arcade.h"
#include "parsec-dso.h"
#include "ACommand.h"
#include <iostream>

class CommandPrivate : public ACommand
{
public:

	/**
	 * @brief Construct a new CommandPrivate object
	 * 
	 * @param config The host config
	 */
	CommandPrivate(const char* msg, Guest& sender, ParsecHostConfig& config)
		: ACommand(msg, sender), _config(config)
	{}

	/**
	 * @brief Run the command
	 * @return true if the command was successful
	 */
	bool run() override {

		if (Config::cfg.room.privateRoom) {
			setReply("The room is already private.");
			return false;
		}

		setReply("The room is now private.");
		_config.publicGame = false;
		Config::cfg.room.privateRoom = true;

		Config::cfg.Save();
		Arcade::instance.deletePost();

		return true;
	}

	/**
	 * @brief Get the prefixes
	 * @return The prefixes
	 */
	static vector<const char*> prefixes() {
		return vector<const char*> { "!private" };
	}

protected:

	/**
	 * @brief Get the internal prefixes
	 * @return The internal prefixes
	 */
	static vector<const char*> internalPrefixes() {
		return vector<const char*> { "!private " };
	}

	ParsecHostConfig& _config;
};




