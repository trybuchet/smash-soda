#pragma once

#include "ACommand.h"

class CommandBotMessage : public ACommand
{
public:

	/**
	 * @brief Construct a new CommandBotMessage object
	 * 
	 * @param msg
	 */
	CommandBotMessage(const char* msg, Guest& sender)
		: ACommand(msg, sender)
	{
		isBotCommand = true;
	}

	/**
	 * @brief Run the command
	 * 
	 * @return true
	 * @return false
	 */
	bool run() override {
		//setReply(_msg.c_str());
		return true;
	}

protected:
};


