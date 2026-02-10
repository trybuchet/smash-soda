#pragma once

#include "ACommand.h"
#include "Hotseat.h"

using namespace std;

class CommandHotseat : public ACommand
{
public:

	/**
	 * @brief Construct a new CommandHotseat object
	 * 
	 * @param sender
	 * @param hotseat
	 */
	CommandHotseat(const char* msg, Guest& sender, Hotseat& hotseat)
		: ACommand(msg, sender), _hotseat(hotseat), _sender(sender)
	{}

	/**
	 * @brief Run the command
	 * 
	 * @return true
	 * @return false
	 */
	bool run() override {

		// Get user permissions
		Tier tier = Cache::cache.tierList.getTier(_sender.userID);

		if (tier > Tier::GUEST && getArgs().size() > 0) {
			
			// Set hotseat mode
			std::string mode = getArgs()[0];
			if (mode == "on" && !_hotseat.running) {
				enableHotseat();
			}
			else if (mode == "off" && _hotseat.running) {
				disableHotseat();
			}
			else if (mode == "toggle") {
				if (_hotseat.running) {
					disableHotseat();
				}
				else {
					enableHotseat();
				}
			} else {
				setReply("Usage: !hotseat [on/off/toggle]");
			}

			return true;

		}

		std::string response = "```\n";

		response += _sender.name + "\n";
		response += "REMAINING TIME: " + _hotseat.getUserTimeRemaining(_sender.id) + "\n";
		response += "COOLDOWN TIME: " + _hotseat.getCooldownRemaining(_sender.id) + "\n";

		response += "```";

	}

	/**
	 * @brief Get the prefixes object
	 * 
	 * @return std::vector<const char*>
	 */
	static vector<const char*> prefixes() {
		return vector<const char*> { "!hotseat", "!hotseatoff", "!hotseaton" };
	}

protected:
	Hotseat& _hotseat;
	Guest& _sender;

	void disableHotseat() {
		_hotseat.stop();
		Config::cfg.hotseat.enabled = false;
		Config::cfg.Save();
		setReply("Hotseat mode disabled");
	}

	void enableHotseat() {
		_hotseat.start();
		Config::cfg.hotseat.enabled = true;
		Config::cfg.Save();
		setReply("Hotseat mode enabled");
	}
};


