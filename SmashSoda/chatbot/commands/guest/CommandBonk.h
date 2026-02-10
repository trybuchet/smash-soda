#pragma once

#include "../../../Core/Config.h"
#include "ACommand.h"
#include <Windows.h>
#include <mmsystem.h>
#include <iostream>
#include <sstream>
#include "parsec.h"
#include "../../../Helpers/Dice.h"
#include "../../../Helpers/Stopwatch.h"

class CommandBonk : public ACommand
{
public:

	/**
	 * Example of how to use the command
	 */
	std::string usage = "!bonk <username>\nExample: !bonk melon";

	/**
	 * Construct a new CommandBonk object
	 * 
	 * @param msg 
	 * @param sender 
	 * @param guests 
	 * @param host 
	 */
	CommandBonk(const char* msg, Guest& sender, GuestList& guestList, Guest& host)
		: ACommand(msg, sender), sender(sender), host(host), guests(guestList)
	{}

	/**
	 * @brief Run the command
	 * @return true if the command was successful
	 */
	bool run() override {

		// Bonk enabled?
		if (Config::cfg.chat.bonkEnabled == false) {
			setReply("Bonk command is disabled.\0");
			return false;
		}

		if (!_stopwatch.isFinished()) {
			setReply("Bonk command is on cooldown: " + std::to_string(_stopwatch.getRemainingTime() / 1000) + " seconds left.\0");
			return false;
		}

		// Was a guest specified?
		if (getArgs().size() == 0) {
			setReply(usage);
			return false;
		}

		// Find the guest
		if (!findGuest()) {
			setReply("Bonk who? They're not here!\0");
			return false;
		}

		int seed = static_cast<unsigned int>(time(NULL));
		srand(seed);
		static uint8_t BONK_CHANCE = 50;

		// Did we bonk ourselves?
		if (target.userID == sender.userID) {
			playHitSound();
			setReply(target.name + " bonked themselves, because they are just so silly.");
			return true;
		}

		// Did we ACTUALLY try to bonk host?
		if (target.userID == host.userID && Config::cfg.chat.hostBonkProof) {
			
			// Pick a random bonk-proof message
			std::string message = _hostBonkProof[rand() % 4];

			// Replace %s with the sender's name
			std::string::size_type pos = message.find("%s");
			if (pos != std::string::npos) {
				message.replace(pos, 2, sender.name);
			}
			setReply(message);
			playDodgeSound();

			return true;
		}

		// Did we bonk the target?
		if (Dice::roll(BONK_CHANCE)) {
			playHitSound();
			setReply(sender.name + " bonked " + target.name + ". *Bonk!*");
		}
		else {
			playDodgeSound();
			setReply(target.name + " dodged " + sender.name + "'s bonk. *Swoosh!*");
		}

		return true;
	}

	/**
	 * @brief Get the prefixes object
	 * @return vector<const char*> 
	 */
	static vector<const char*> prefixes() {
		return vector<const char*> { "!bonk" };
	}

	/**
	 * @brief Initialize the command
	 */
	static void init();

protected:
	static vector<const char*> internalPrefixes() {
		return vector<const char*> { "!bonk " };
	}

	void playHitSound() {
		try {
			PlaySound(TEXT("./SFX/bonk-hit.wav"), NULL, SND_FILENAME | SND_NODEFAULT | SND_ASYNC);
		}
		catch (const std::exception&) {}
	}

	void playDodgeSound() {
		try {
			PlaySound(TEXT("./SFX/bonk-dodge.wav"), NULL, SND_FILENAME | SND_NODEFAULT | SND_ASYNC);
		}
		catch (const std::exception&) {}
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
	Guest& host;
	GuestList guests;

	static Stopwatch _stopwatch;

	/**
	 * @brief Bonk-proof messages
	 */
	string _hostBonkProof[4] = {
		"%s tried to bonk the host, but the host laughs at your puny efforts.\0",
		"%s tried to bonk the host, but the host's power level is 9000!!!!\0",
		"%s tried to bonk the host, but the host is just too good for you, so don't even try.\0",
		"The host effortlessly sidesteps %s's bonk attempt.\0",
	};
};


