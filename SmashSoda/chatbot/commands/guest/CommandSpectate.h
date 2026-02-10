#pragma once

#include "Cache.h"
#include "ACommand.h"
#include "Hotseat.h"

using namespace std;

class CommandSpectate : public ACommand
{
public:

	/**
	 * @brief Construct a new CommandSpectate object
	 * 
	 * @param msg 
	 * @param sender 
	 * @param guests 
	 * @param gamepadClient 
	 * @param hotseat 
	 */
	CommandSpectate(const char* msg, Guest& sender, GuestList& guests, GamepadClient& gamepadClient, Hotseat& hotseat)
		: ACommand(msg, sender), _gamepadClient(gamepadClient), _hotseat(hotseat), guests(guests), sender(sender)
	{}

	/**
	 * @brief Run the command
	 * 
	 * @return true 
	 * @return false 
	 */
	bool run() override {

		// If guest name not provided
		if (getArgs().size() == 0) {
			addGuestToSpectators(_sender);
		} else {
			if (findGuest()) {
				addGuestToSpectators(target);
			}
		}

		isBotCommand = true;
		return true;
	}

	/**
	 * @brief Add a guest to the spectators list
	 * 
	 * @param guest 
	 * @return true 
	 * @return false 
	 */
	bool addGuestToSpectators(Guest guest) {

		GuestData guestData = GuestData(guest.name, guest.userID);

		// if (MetadataCache::isSpectating(guest.userID)) {

		// 	MetadataCache::addActiveGuest(guest);
		// 	GuestData guestData = GuestData(guest.name, guest.userID);
		// 	_replyMessage = Config::cfg.chatbotName + guest.name + " is no longer spectating.\0";
			
		// }
		// else {

		// 	MetadataCache::removeActiveGuest(guest);
		// 	_replyMessage = Config::cfg.chatbotName + guest.name + " is now spectating.\0";
		// 	_gamepadClient.onQuit(guest);

		// }

		return true;

	}

	/**
	 * @brief Get the prefixes object
	 * 
	 * @return vector<const char*> 
	 */
	static vector<const char*> prefixes() {
		return vector<const char*> { "!spectate", "!afk", "!lurk" };
	}

protected:
	static vector<const char*> internalPrefixes()
	{
		return vector<const char*> { "!spectate " };
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
	GamepadClient& _gamepadClient;
	Hotseat& _hotseat;
};


