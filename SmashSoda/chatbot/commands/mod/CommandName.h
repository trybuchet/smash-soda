#pragma once

#include "Arcade.h"
#include "parsec-dso.h"
#include "ACommand.h"

class CommandName : public ACommand
{
public:

	std::string usage = "Usage: !name <name>\nExample: !name Smash Bros Ultimate";
	
	/**
	 * @brief Construct a new CommandName object
	 * 
	 * @param msg
	 * @param config
	 */
	CommandName(const char* msg, Guest& sender, ParsecHostConfig &config)
		: ACommand(msg, sender), _config(config)
	{}

	/**
	 * @brief Run the command
	 * 
	 * @return true
	 * @return false
	 */
	bool run() override {

		if (getArgs().size() < 1) {
			setReply(usage);
			return false;
		}
		
		std::string name = getArgs()[0];

		Config::cfg.room.game = name;
		strcpy_s(_config.name, name.c_str());

		Config::cfg.Save();
		Arcade::instance.createPost();

		return true;
	}

	/**
	 * @brief Get the prefixes object
	 * 
	 * @return std::vector<const char*>
	 */
	static vector<const char*> prefixes() {
		return vector<const char*> { "!name" };
	}

protected:


	static vector<const char*> internalPrefixes() {
		return vector<const char*> { "!name " };
	}
	ParsecHostConfig& _config;
};



