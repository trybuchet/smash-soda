#pragma once

#include "../../../Core/Config.h"
#include "Cache.h"
#include "parsec-dso.h"
#include "ACommand.h"

using namespace std;

class CommandSFX : public ACommand
{
public:
	
	/**
	 * @brief Construct a new CommandSFX object
	 * 
	 * @param sender 
	 */
	CommandSFX(const char* msg, Guest& sender)
		: ACommand(msg, sender), _sender(sender)
	{}

	/**
	 * @brief Run the command
	 * 
	 * @return true 
	 * @return false 
	 */
	bool run() override {

		// Tier list enabled?
		Tier tier = Cache::cache.tierList.getTier(_sender.userID);

		// SFX enabled?
		if (tier == Tier::GUEST && !Config::cfg.permissions.guest.useSFX ||
			tier == Tier::MOD && !Config::cfg.permissions.moderator.useSFX ||
			Cache::cache.vipList.isVIP(_sender.userID) && !Config::cfg.permissions.vip.useSFX) {
			setReply("You do not have permission to use this command.\0");
			return false;
		}

		if (Cache::cache.sfxList.size() <= 0) {
			setReply("No sound effects available.\0");
			return false;
		}

		// Was a sfx given specified?
		if (getArgs().size() == 0) {
			string reply =
				string("Usage: !sfx <sound name> | Example: !sfx bruh\n") +
				string("List of available sound names:\n") +
				Cache::cache.sfxList.loadedTags() +
				string("\0");
			setReply(reply);
			return false;
		}

		string sound = getArgString();
		transform(sound.begin(), sound.end(), sound.begin(), ::tolower);
		SFXList::SFXPlayResult result = Cache::cache.sfxList.play(sound);

		switch (result) {
		case SFXList::SFXPlayResult::COOLDOWN:
			setReply(string("Command !sfx is on cooldown: ") +
				to_string(Cache::cache.sfxList.getRemainingCooldown()) +
				string(" seconds left."));
			break;
		case SFXList::SFXPlayResult::NOT_FOUND:
				setReply("Sound effect not found.");
			break;
		case SFXList::SFXPlayResult::OK:
		default:
			setReply("Playing sound effect: " + sound);
			break;
		}

		return true;
	}

	/**
	 * @brief Get the prefixes object
	 * 
	 * @return vector<const char*> 
	 */
	static vector<const char*> prefixes() {
		return vector<const char*> { "!sfx" };
	}

protected:
	Guest& _sender;

	static vector<const char*> internalPrefixes() {
		return vector<const char*> { "!sfx " };
	}
};



