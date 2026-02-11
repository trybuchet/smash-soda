#pragma once

#include "ACommand.h"
#include "AutoMod.h"

class CommandUnmute : public ACommand
{
public:

	std::string usage = "Usage: !unmute <username> \nExample: !unmute MickeyUK\0";

	/**
	 * @brief Construct a new CommandMute object
	 *
	 * @param msg
	 * @param sender
	 * @param guests
	 * @param host
	 */
	CommandUnmute(const char* msg, Guest& sender, GuestList& guests, Guest& host)
		: ACommand(msg, sender), _sender(sender), _host(host), guests(guests)
	{}

	/**
	 * @brief Run the command
	 *
	 * @return true
	 * @return false
	 */
	bool run() override {

		// Was a guest specified?
		if (getArgs().size() == 0) {
			setReply(usage);
			return false;
		}

		// Find the guest
		if (!findGuest()) {
			setReply("Can't find the guest you want to mute!\0");
			return false;
		}

		AutoMod::instance.UnmuteUser(target.userID);
		setReply(target.name + " has been unmuted\0");

		return true;
	}

	/**
	 * @brief Get the prefixes object
	 *
	 * @return std::vector<const char*>
	 */
	static vector<const char*> prefixes() {
		return vector<const char*> { "!mute" };
	}

protected:
	static vector<const char*> internalPrefixes() {
		return vector<const char*> { "!mute " };
	}

	/**
	* Get the guest referenced in the command. Returns nullptr
	* if no guest is found
	*
	* @param guestList The guest list
	*/
	bool findGuest() {

		// Get the guest
		string guest = getArgString();
		if (guest == "") {
			return false;
		}

		try {
			uint32_t id = stoul(guest);
			vector<Guest>::iterator i;
			for (i = guests.getGuests().begin(); i != guests.getGuests().end(); ++i) {
				if ((*i).userID == id) {
					target = *i;
					return true;
				}
			}
		}
		catch (const std::exception&) {
			bool found = guests.find(guest, &target);
			if (found) {
				return true;
			}
		}

		return false;
	}

	Guest target;
	GuestList guests;
	Guest& _sender;
	Guest& _host;
};


