#pragma once

#include "Cache.h"
#include "ACommand.h"
#include "parsec-dso.h"

using namespace std;

class CommandBan : public ACommand
{
public:

	std::string usage = "Usage: !ban <username>\nExample: !ban MK10\0";

	/**
	 * @brief Construct a new CommandBan object
	 * 
	 * @param msg 
	 * @param sender 
	 * @param parsec 
	 * @param guests 
	 * @param guestHistory 
	 */
	CommandBan(const char* msg, Guest& sender, ParsecDSO* parsec, GuestList &guests, GuestDataList &guestHistory)
		: ACommand(msg, sender), _parsec(parsec), guests(guests), _guestHistory(guestHistory)
	{
	}

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
		if (findGuest()) {
			GuestData targetData(target.name, target.userID);
			targetData.fake = target.fake;
			return handleGuest(targetData, true, target.id);
		}

		// Find offline guest
		string guest = getArgString();
		if (guest == "") {
			return false;
		}
		bool found = false;
		try {
			found = _guestHistory.find(guest, [&](GuestData& guest) { _offlineGuest = guest; });
		}
		catch (const std::exception&) {}
		if (found) {
			return handleGuest(_offlineGuest, false);
		}
		return false;

	}

	/**
	 * @brief Get the prefixes object
	 * 
	 * @return vector<const char*> 
	 */
	static vector<const char*> prefixes() {
		return vector<const char*> { "!ban", "!block" };
	}

private:
	static vector<const char*> internalPrefixes() {
		return vector<const char*> { "!ban " };
	}

	ParsecDSO* _parsec;
	Guest target;
	GuestList guests;
	GuestDataList& _guestHistory;
	GuestData _offlineGuest;

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

	/**
	 * @brief Handle the guest
	 * 
	 * @param target 
	 * @param isOnline 
	 * @param guestID 
	 * @return true 
	 * @return false 
	 */
	bool handleGuest(GuestData target, bool isOnline, uint32_t guestID = -1) {
		bool result = false;

		if (_sender.userID == target.userID) {
			setReply("Thou shall not ban thyself, " + _sender.name + " ...\0");
		} 
		
		else if (Cache::cache.isSodaCop(target.userID)) {
			setReply("Nice try bub, but you can't ban a Soda Cop!\0");
		} else {
			setReply(
				Cache::cache.isSodaCop(_sender.userID) ?
				_sender.name + " laid down the law as a Soda Cop and banned " + target.name + "!\0"
				: _sender.name + " banned " + target.name + "!\0"
			);

			if (Cache::cache.banList.ban(target)) {
				if (isOnline) {

					if (target.fake) {
						vector<Guest>::iterator i;
						for (i = guests.getGuests().begin(); i != guests.getGuests().end(); ++i) {
							if ((*i).userID == target.userID) {
								guests.getGuests().erase(i);
								break;
							}
						}
					} else {
						ParsecHostKickGuest(_parsec, guestID);
					}
				}

				try {
					PlaySound(TEXT("./SFX/ban.wav"), NULL, SND_FILENAME | SND_NODEFAULT | SND_ASYNC);
				}
				catch (const std::exception&) {}

				result = true;
			}
		}

		return result;
	}
};


