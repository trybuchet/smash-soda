#pragma once

#include "ACommand.h"
#include "../../../core/GamepadClient.h"

class CommandStrip : public ACommand
{
public:

	/**
	 * @brief Construct a new CommandSwap object
	 *
	 * @param msg
	 * @param sender
	 * @param gamepadClient
	 */
	CommandStrip(const char* msg, Guest& sender, GamepadClient& gamepadClient, GuestList guests)
		: ACommand(msg, sender), _gamepadClient(gamepadClient), guests(guests)
	{}

	/**
	 * @brief Run the command
	 *
	 * @return true
	 * @return false
	 */
	bool run() override {

		size_t nGamepads = _gamepadClient.gamepads.size();

		if (nGamepads == 0) {
			setReply("No gamepads available.\0");
			return false;
		}

		if (getArgs().size() == 0) {
			setReply("Usage: !strip <guest name | integer in range [1, " + std::to_string(nGamepads) + "]>\nExample: !strip 4\0");
			return false;
		}

		if (findGuest()) {
			std::vector<AGamepad*>::iterator gi = _gamepadClient.gamepads.begin();

			int i = 0;
			for (; gi != _gamepadClient.gamepads.end(); ++gi) {
				if ((*gi)->isConnected() && (*gi)->isOwned()) {
					if ((*gi)->owner.guest.userID == target.userID) {
						_gamepadClient.clearOwner(i);
						setReply("Taken away gamepad from " + target.name + ".\0");
						return true;
					}
				}
				i++;
			}

		}

		int slot = 1;
		try {
			slot = std::stoi(getArgs()[0]);
		}
		catch (std::invalid_argument) {
			setReply("Usage: !swap <integer in range [1, " + std::to_string(nGamepads) + "]>\nExample: !strip 4\0");
			return false;
		}

		// In range [1, nGamepads]
		if (slot < 1 || slot > nGamepads) {
			setReply("Usage: !swap <integer in range [1, " + std::to_string(nGamepads) + "]>\nExample: !strip 4\0");
			return false;
		}

		bool success = _gamepadClient.clearOwner(slot - 1);
		if (!success) {
			setReply("Usage: !strip <integer in range [1, 4]>\nExample: !strip 4\0");
			return false;
		}

		return true;
	}

	/**
	 * @brief Get the prefixes object
	 *
	 * @return vector<const char*>
	 */
	static vector<const char*> prefixes() {
		return vector<const char*> { "!strip", "!take" };
	}

protected:
	static vector<const char*> internalPrefixes()
	{
		return vector<const char*> { "!strip " };
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
	string _msg;
	GamepadClient& _gamepadClient;
};


