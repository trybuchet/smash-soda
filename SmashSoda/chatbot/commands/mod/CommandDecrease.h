#pragma once

#include "ACommand.h"
#include "Hotseat.h"

class CommandDecrease : public ACommand
{
public:

	std::string usage = "Usage: !decrease <username> <minutes>\nExample: !decrease RefffiK 6\0";

	/**
	 * @brief Construct a new CommandDecrease object
	 * 
	 * @param msg
	 * @param sender
	 * @param guests
	 * @param host
	 */
	CommandDecrease(const char* msg, Guest& sender, GuestList& guests, Guest& host)
		: ACommand(msg, sender), _host(host), guests(guests)
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
			setReply("Can't find the guest!\0");
			return false;
		}

		int amount = 0;
		try {
			amount = std::stoi(getArgs()[1]);
		}
		catch (std::invalid_argument) {
			setReply(usage);
			return false;
		}
		Hotseat::instance.reduceUserPlaytime(target.userID, amount);
		setReply("Decreased hotseat time for " + target.name + " by " + std::to_string(amount) + " minutes\0");

		return false;
	}

	/**
	 * @brief Get the prefixes object
	 * 
	 * @return std::vector<const char*>
	 */
	static vector<const char*> prefixes() {
		return vector<const char*> { "!decrease", "!taketime" };
	}

protected:
	static vector<const char*> internalPrefixes()
	{
		return vector<const char*> { "!decrease " };
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

	Guest& _host;
};


