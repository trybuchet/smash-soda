#pragma once

#include "ACommand.h"
#include "../../../Guest.h"

class CommandStopSFX : public ACommand
{
public:

	/**
	 * @brief Construct a new CommandStopSFX object
	 * 
	 * @param sender 
	 */
	CommandStopSFX(const char* msg, Guest& sender)
		: ACommand(msg, sender), _sender(sender)
	{}

	/**
	 * @brief Run the command
	 * @return true if the command was successful
	 */
	bool run() override {


		Tier tier = Cache::cache.tierList.getTier(_sender.userID);

		// SFX enabled?
		if (tier == Tier::GUEST && !Config::cfg.permissions.guest.useSFX ||
			tier == Tier::MOD && !Config::cfg.permissions.moderator.useSFX ||
			Cache::cache.vipList.isVIP(_sender.userID) && !Config::cfg.permissions.vip.useSFX) {
			setReply("Sound effects are disabled.\0");
			return false;
		} else { 
			PlaySound(NULL, NULL, SND_FILENAME | SND_NODEFAULT | SND_ASYNC | SND_SYSTEM);
		}
		return true;
	}


	/**
	 * @brief Get the prefixes
	 * @return vector<const char*> 
	 */
	static vector<const char*> prefixes() {
		return vector<const char*> { "!stop" };
	}

protected:

	Guest& _sender;
};


