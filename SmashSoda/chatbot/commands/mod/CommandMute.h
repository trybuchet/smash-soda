#pragma once

#include "ACommand.h"
#include "AutoMod.h"

class CommandMute : public ACommand
{
public:

	std::string usage = "Usage: !mute <username> <minutes>\nExample: !mute Call_Me_Troy 5\0";

	/**
	 * @brief Construct a new CommandMute object
	 * 
	 * @param msg
	 * @param sender
	 * @param guests
	 * @param host
	 */
	CommandMute(const char* msg, Guest& sender, GuestList& guests, Guest& host)
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
		if (getArgs().size() < 2) {
			setReply(usage);
			return false;
		}

		// Find the guest
		if (!findGuest()) {
			setReply("Can't find the guest you want to mute!\0");
			return false;
		}

		if (_sender.userID == target.userID) {
			setReply(_sender.name + " tried to mute...themselves?");
		}
		else {
			int duration = 5;
			try {
				duration = stoi(getArgs()[1]);
			}
			catch (const std::exception&) {
				setReply(usage);
				return false;
			}
			AutoMod::instance.MuteUser(target.userID, target.name, duration);
			setReply("Shhh, "+ target.name + "! They have been muted for " + to_string(duration) + " minutes.");
		}
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


