#pragma once

#include "ACommand.h"

class CommandQuit : public ACommand
{
public:

	/**
	 * @brief Construct a new CommandQuit object
	 * 
	 * @param hostingLoopController The hosting loop controller
	 */
	CommandQuit(const char* msg, Guest& sender, bool & hostingLoopController)
		: ACommand(msg, sender), _hostingLoopController(hostingLoopController)
	{}

	/**
	 * @brief Run the command
	 * @return true if the command was successful
	 */
	bool run() override {
		setReply("Goodbye!");
		_hostingLoopController = false;
		return true;
	}

	/**
	 * @brief Get the prefixes
	 * @return The prefixes
	 */
	static vector<const char*> prefixes() {
		return vector<const char*> { "!q" };
	}

protected:
	bool& _hostingLoopController;
};




