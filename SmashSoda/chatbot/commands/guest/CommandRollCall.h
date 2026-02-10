#pragma once

#include "../../../Core/Config.h"
#include "ACommand.h"
#include "../../../core/GuestList.h"

using namespace std;

class CommandRollCall : public ACommand {
public:

	/**
	 * @brief Construct a new CommandRollCall object
	 * 
	 * @param msg 
	 * @param sender 
	 * @param guestList 
	 */
	CommandRollCall(const char* msg, Guest& sender, GuestList& guestList)
		: ACommand(msg, sender), _guestList(guestList) {}

	/**
	 * @brief Run the command
	 * @return true if the command was successful
	 */
	bool run() override {

		string reponse = "[ACTIVE PLAYERS]\n";

		// For each guest in the guest list, send a message to them.
		for (auto& guest : _guestList.getGuests()) {
			reponse += "[" + guest.name + "]\n";
		}

		_replyMessage = reponse;
		isBotCommand = true;

		return true;

	}

	/**
	 * @brief Get the prefixes object
	 * @return vector<const char*> 
	 */
	static vector<const char*> prefixes() {
		return vector<const char*> { "!rollcall" };
	}

protected:
	GuestList& _guestList;

	static vector<const char*> internalPrefixes() {
		return vector<const char*> { "!rollcall " };
	}
};


