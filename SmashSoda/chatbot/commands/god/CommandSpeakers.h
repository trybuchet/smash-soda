#pragma once

#include <sstream>
#include "ACommand.h"
#include "../../../AudioOut.h"

class CommandSpeakers : public ACommand
{
public:

	std::string usage = "Usage: !speakers <integer in range [0, 100]>\nExample: !speakers 42\0";

	/**
	 * @brief Construct a new CommandSpeakers object
	 * 
	 * @param msg The message to parse
	 * @param audioOut The audio out
	 */
	CommandSpeakers(const char* msg, Guest& sender, AudioOut& audioOut)
		: ACommand(msg, sender), _audioOut(audioOut)
	{}

	/**
	 * @brief Run the command
	 * @return true if the command was successful
	 */
	bool run() override {

		if (getArgs().size() == 0) {
			setReply(usage);
			return false;
		}

		int value = 70;
		try {
			value = std::stoi(getArgs()[0]);
		}
		catch (std::invalid_argument) {
			setReply(usage);
			return false;
		}

		// Check if the value is in range
		if (value < 0 || value > 100) {
			setReply(usage);
			return false;
		}

		_audioOut.volume = (float)value / 100.0f;
		setReply("Speakers volume set to " + std::to_string(value) + "%\0");

		return true;
	}

	/**
	 * @brief Get the prefixes
	 * @return The prefixes
	 */
	static vector<const char*> prefixes() {
		return vector<const char*> { "!speakers" };
	}

protected:
	static vector<const char*> internalPrefixes()
	{
		return vector<const char*> { "!speakers " };
	}

	AudioOut& _audioOut;
};



