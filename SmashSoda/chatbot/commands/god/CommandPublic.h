#pragma once

#include "Arcade.h"
#include "parsec-dso.h"
#include "ACommand.h"
#include <iostream>

class CommandPublic : public ACommand
{
public:

	/**
	 * @brief Construct a new CommandPublic object
	 * 
	 * @param config The host config
	 */
	CommandPublic(const char* msg, Guest& sender, ParsecHostConfig& config)
		: ACommand(msg, sender), _config(config)
	{}

	/**
	 * @brief Run the command
	 * @return true if the command was successful
	 */
	bool run() override {
		if (!Config::cfg.room.privateRoom) {
			setReply("The room is already public.");
			return false;
		}

		setReply("The room is now private.");
		_config.publicGame = true;
		Config::cfg.room.privateRoom = false;

		Config::cfg.Save();
		Arcade::instance.createPost();
	}

	/**
	 * @brief Get the prefixes
	 * @return The prefixes
	 */
	static vector<const char*> prefixes(){
		return vector<const char*> { "!public" };
	}

protected:

	/**
	 * @brief Get the internal prefixes
	 * @return The internal prefixes
	 */
	static vector<const char*> internalPrefixes() {
		return vector<const char*> { "!public " };
	}

	ParsecHostConfig& _config;
};




