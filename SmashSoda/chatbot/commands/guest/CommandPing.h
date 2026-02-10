#pragma once

#include "ACommand.h"
#include "parsec.h"

class CommandPing : public ACommand
{
public:

	std::string usage = "Usage: !ping <guest>\0";

	/**
	 * @brief Construct a new CommandPing object
	 * 
	 * @param msg 
	 * @param sender 
	 * @param guests 
	 * @param host 
	 */
	CommandPing(const char* msg, Guest& sender, GuestList& guests, Guest& host, GuestList& guestList)
		: ACommand(msg, sender), _host(host), guests(guestList)
	{}

	/**
	 * @brief Run the command
	 * @return true if the command was successful
	 */
	bool run() override {

		// Was a guest specified?
		if (getArgs().size() == 0) {
			setReply(_sender.name + ": [" + getLatency(_sender.id) + "ms]\0");
			return false;
		}

		// Find the guest
		if (!findGuest()) {
			setReply("Can't find the guest you want to check!\0");
			return false;
		}

		setReply(target.name + ": [" + getLatency(target.id) + "ms]\0");

		return false;
	}

	/**
	 * @brief Get the prefixes object
	 * @return vector<const char*> 
	 */
	static vector<const char*> prefixes() {
		return vector<const char*> { "!ping", "!latency" };
	}

protected:
	static vector<const char*> internalPrefixes(){
		return vector<const char*> { "!ping " };
	}

	std::string getLatency(uint32_t id) {
		ParsecMetrics metrics = guests.getMetrics(id).metrics;
		stringstream ping;
		ping << trunc(metrics.networkLatency);

		return ping.str();
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
	Guest& _host;
	GuestList guests;
};


