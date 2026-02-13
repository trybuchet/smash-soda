#pragma once

#include "ACommand.h"
#include "ProcessMan.h"

using namespace std;

class CommandRestart : public ACommand
{
public:
	string filename;

	/**
	 * @brief Construct a new CommandRestart object
	 * 
	 */
	CommandRestart(const char* msg, Guest& sender) : 
		ACommand(msg, sender) {}

	/**
	 * @brief Run the command
	 * 
	 * @return true
	 * @return false
	 */
	bool run() override {

		if (Config::cfg.kioskMode.enabled) {
			setReply("The application is restarting...wait!");
			ProcessMan::instance.restart();
		}
		else {
			setReply("The host does not have kiosk mode enabled.");
		}
		
		return true;
	}

	/**
	 * @brief Get the prefixes object
	 * 
	 * @return std::vector<const char*>
	 */
	static vector<const char*> prefixes() {
		return vector<const char*> { "!restart", "!kill" };
	}
};


