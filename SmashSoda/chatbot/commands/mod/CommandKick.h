#pragma once

#include "ACommand.h"
#include "parsec-dso.h"


class CommandKick : public ACommand
{
public:

	std::string usage = "Usage: !kick <username>\nExample: !kick bigboi83";

	/**
	 * @brief Construct a new CommandKick object
	 *
	 * @param msg
	 * @param sender
	 * @param parsec
	 * @param guests
	 * @param isHost
	 */
	CommandKick(const char* msg, Guest& sender, ParsecDSO* parsec, GuestList& guests, bool isHost = false)
		: ACommand(msg, sender), _parsec(parsec), _isHost(isHost), guests(guests), sender(sender)
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
			setReply("Can't find the guest you want to kick!\0");
			return false;
		}

		// Kicked themselves...?
		if (sender.userID == target.userID) {
			setReply(sender.name + " kicked themselves in confusion!");
			kickGuest();
			return true;
		}

		// Can't kick a Soda Cop
		if (Cache::cache.isSodaCop(target.userID)) {
			setReply("Nice try bub, but you can't kick a Soda Cop!\0");
			return false;
		}

		// Kick the guest
		setReply(
			Cache::cache.isSodaCop(sender.userID) ?
			sender.name + " laid down the law as a Soda Cop and kicked " + target.name + "!\0"
			: sender.name + " kicked " + target.name + "!\0"
		);
		kickGuest();

		try {
			PlaySound(TEXT("./SFX/kick.wav"), NULL, SND_FILENAME | SND_NODEFAULT | SND_ASYNC);
		}
		catch (const std::exception&) {}

		return true;
	}

	/**
	 * @brief Get the prefixes object
	 *
	 * @return vector<const char*>
	 */
	static vector<const char*> prefixes() {
		return vector<const char*> { "!kick" };
	}

protected:
	static vector<const char*> internalPrefixes() {
		return vector<const char*> { "!kick " };
	}

	void kickGuest() {
		if (target.fake) {
			for (int i = 0; i < guests.getGuests().size(); ++i) {
				if (guests.getGuests()[i].userID == target.userID) {
					guests.getGuests().erase(guests.getGuests().begin() + i);
					break;
				}
			}
			return;
		}
		ParsecHostKickGuest(_parsec, target.id);
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
	Guest& sender;
	GuestList guests;
	ParsecDSO* _parsec;
	bool _isHost;
};


