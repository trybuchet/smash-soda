#include "ChatBot.h"
#include "../core/GamepadClient.h"

ACommand* ChatBot::identifyUserDataMessage(const char* msg, Guest& sender, bool isHost) {
	const uint32_t previous = this->_lastUserId;
	setLastUserId(BOT_GUESTID);

	// Get the user's tier and permissions
	Tier tier = Cache::cache.tierList.getTier(sender.userID);

	// Is this a Soda Cop, laying down the law?
	if (!isHost && Cache::cache.isSodaCop(sender.userID)) {

		if (isCommand(msg, CommandKick::prefixes())) {
			return new CommandKick(msg, sender, _parsec, _guests, isHost);
		}

		if (isCommand(msg, CommandBan::prefixes())) {
			return new CommandBan(msg, sender, _parsec, _guests, _guestHistory);
		}

		/*if (msgIsEqual(msg, CommandVersion::prefixes()))		return new CommandVersion(sender);*/
	}

	// Is this a custom command?
	ACommand* custom = _chatBotCustom->isCustomCommand(msg, sender, isHost, tier, previous);
	if (custom->isBotCommand) {
		return custom;
	}

	/*
	REGULAR USER COMMANDS
	Any user can use these commands. All commands require msg and sender
	args, but some commands may require additional args.
	*/

	if (isCommand(msg, Command8Ball::prefixes())) {
		return new Command8Ball(msg, sender);
	}

	if (isCommand(msg, CommandBB::prefixes())) {
		return new CommandBB(msg, sender, GamepadClient::instance, _macro);
	}

	if (isCommand(msg, CommandBonk::prefixes())) {
		return new CommandBonk(msg, sender, _guests, _host);
	}

	if (isCommand(msg, CommandDiscord::prefixes())) {
		return new CommandDiscord(msg, sender);
	}

	if (isCommand(msg, CommandFF::prefixes())) {
		return new CommandFF(msg, sender, GamepadClient::instance, _hotseat);
	}

	if (isCommand(msg, CommandFortune::prefixes())) {
		return new CommandFortune(msg, sender);
	}

	if (isCommand(msg, CommandHelp::prefixes())) {
		return new CommandHelp(msg, sender, commands);
	}

	if (isCommand(msg, CommandHotseat::prefixes())) {
		return new CommandHotseat(msg, sender, _hotseat);
	}

	if (isCommand(msg, CommandKeyboard::prefixes())) {
		return new CommandKeyboard(msg, sender, GamepadClient::instance);
	}

	if (isCommand(msg, CommandMirror::prefixes())) {
		return new CommandMirror(msg, sender, GamepadClient::instance);
	}

	if (isCommand(msg, CommandPads::prefixes())) {
		return new CommandPads(msg, sender, GamepadClient::instance);
	}

	if (isCommand(msg, CommandSpectate::prefixes())) {
		return new CommandSpectate(msg, sender, _guests, GamepadClient::instance, _hotseat);
	}

	if (isCommand(msg, CommandSwap::prefixes())) {
		return new CommandSwap(msg, sender, GamepadClient::instance);
	}

	if (isCommand(msg, CommandTriangle::prefixes())) {
		return new CommandTriangle(msg, sender, GamepadClient::instance, _macro);
	}

	if (isCommand(msg, CommandPing::prefixes())) {
		return new CommandPing(msg, sender, _guests, _host, _guests);
	}

	if (isCommand(msg, CommandRPG::prefixes())) {
		return new CommandRPG(msg, sender, _guests);
	}

	if (isCommand(msg, CommandRollCall::prefixes())) {
		return new CommandRollCall(msg, sender, _guests);
	}

	if (isCommand(msg, CommandSFX::prefixes())) {
		return new CommandSFX(msg, sender);
	}

	if (isCommand(msg, CommandStopSFX::prefixes())) {
		return new CommandStopSFX(msg, sender);
	}

	/*
	MODERATOR COMMANDS
	The host and moderators can use these commands. Moderators have the
	"MOD" tier. The host has the "GOD" tier.
	*/
	if (tier >= Tier::MOD || isHost) {

		if (isCommand(msg, CommandBan::prefixes())) {
			return new CommandBan(msg, sender, _parsec, _guests, _guestHistory);
		}
		if (isCommand(msg, CommandDC::prefixes())) {
			return new CommandDC(msg, sender, GamepadClient::instance);
		}
		if (isCommand(msg, CommandDecrease::prefixes())) {
			return new CommandDecrease(msg, sender, _guests, _host);
		}
		if (isCommand(msg, CommandExtend::prefixes())) {
			return new CommandExtend(msg, sender, _guests, _host);
		}
		if (isCommand(msg, CommandWarmup::prefixes())) {
			return new CommandWarmup(msg, sender, _guests, _host);
		}
		if (isCommand(msg, CommandCooldown::prefixes())) {
			return new CommandCooldown(msg, sender, _guests, _host);
		}
		if (isCommand(msg, CommandKick::prefixes())) {
			return new CommandKick(msg, sender, _parsec, _guests, isHost);
		}
		if (isCommand(msg, CommandLock::prefixes())) {
			return new CommandLock(msg, sender, GamepadClient::instance);
		}
		if (isCommand(msg, CommandLockAll::prefixes())) {
			return new CommandLockAll(msg, sender, GamepadClient::instance);
		}
		if (isCommand(msg, CommandMute::prefixes())) {
			return new CommandMute(msg, sender, _guests, _host);
		}
		if (isCommand(msg, CommandRC::prefixes())) {
			return new CommandRC(msg, sender, GamepadClient::instance);
		}
		if (isCommand(msg, CommandRestart::prefixes())) {
			return new CommandRestart(msg, sender);
		}
		if (isCommand(msg, CommandStrip::prefixes())) {
			return new CommandStrip(msg, sender, GamepadClient::instance, _guests);
		}
		if (isCommand(msg, CommandStripAll::prefixes())) {
			return new CommandStripAll(msg, sender, GamepadClient::instance);
		}
		if (isCommand(msg, CommandUnban::prefixes())) {
			return new CommandUnban(msg, sender, _parsec, _guests, _guestHistory);
		}
		if (isCommand(msg, CommandUnbanLastIP::prefixes())) {
			return new CommandUnbanLastIP(msg, sender);
		}
		if (isCommand(msg, CommandUnmute::prefixes())) {
			return new CommandUnmute(msg, sender, _guests, _host);
		}
		if (isCommand(msg, CommandVerify::prefixes())) {
			return new CommandVerify(msg, sender, _guests, _guestHistory);
		}
		if (isCommand(msg, CommandDCAll::prefixes())) {
			return new CommandDCAll(msg, sender, GamepadClient::instance);
		}
		if (isCommand(msg, CommandName::prefixes())) {
			return new CommandName(msg, sender, _hostConfig);
		}

	}

	/*
	HOST COMMANDS
	Only the host can use these commands. The host has the "GOD" tier.
	*/
	if (tier >= Tier::GOD || isHost) {

		if (isCommand(msg, CommandAddXbox::prefixes())) {
			return new CommandAddXbox(msg, sender, GamepadClient::instance);
		}
		if (isCommand(msg, CommandAddPS::prefixes())) {
			return new CommandAddPS(msg, sender, GamepadClient::instance);
		}
		if (isCommand(msg, CommandPrivate::prefixes())) {
			return new CommandPrivate(msg, sender, _hostConfig);
		}
		if (isCommand(msg, CommandPublic::prefixes())) {
			return new CommandPublic(msg, sender, _hostConfig);
		}
		if (isCommand(msg, CommandSpeakers::prefixes())) {
			return new CommandSpeakers(msg, sender, _audioOut);
		}
		if (isCommand(msg, CommandQuit::prefixes())) {
			return new CommandQuit(msg, sender, _hostingLoopController);
		}
		if (isCommand(msg, CommandMod::prefixes())) {
			return new CommandMod(msg, sender, _guests, _guestHistory);
		}
		if (isCommand(msg, CommandUnmod::prefixes())) {
			return new CommandUnmod(msg, sender, _guests, _guestHistory);
		}
		if (isCommand(msg, CommandUnVIP::prefixes())) {
			return new CommandUnVIP(msg, sender, _guests, _guestHistory);
		}
		if (isCommand(msg, CommandVIP::prefixes())) {
			return new CommandVIP(msg, sender, _guests, _guestHistory);
		}
	}

	this->setLastUserId(previous);
	return new CommandDefaultMessage(msg, sender, _lastUserId, tier, isHost);
}

/*
Add all the help/descriptions for the commands.
*/
void ChatBot::addHelp() {
	addCmdHelp("!8ball", "Ask the magic 8-ball a question.", Tier::GUEST);
	addCmdHelp("!bb", "Press the B button twice for everyone.", Tier::GUEST);
	addCmdHelp("!bonk", "Bonk a user.", Tier::GUEST);
	addCmdHelp("!discord", "Get the Discord invite link.", Tier::GUEST);
	addCmdHelp("!ff", "Drop your pad.", Tier::GUEST);
	addCmdHelp("!cookie", "Get a fortune.", Tier::GUEST);
	addCmdHelp("!kb", "Map custom keyboard controls.", Tier::GUEST);
	addCmdHelp("!mirror", "Toggle mirroring of L-Stick into DPad.", Tier::GUEST);
	addCmdHelp("!pads", "List who are holding gamepads.", Tier::GUEST);
	addCmdHelp("!spectate", "Add yourself to spectator list.", Tier::GUEST);
	addCmdHelp("!swap", "Swap your gamepad for another.", Tier::GUEST);
	addCmdHelp("!triangle", "Press the Triangle button for everyone.", Tier::GUEST);
	addCmdHelp("!ping", "Check a guest's latency to the host.", Tier::GUEST);
	addCmdHelp("!playtime", "Check hotseat playtime.", Tier::GUEST);
	addCmdHelp("!rpg", "Silly chat game.", Tier::GUEST);
	addCmdHelp("!rollcall", "List who is not spectating.", Tier::GUEST);
	addCmdHelp("!sfx", "Play a sound effect.", Tier::GUEST);
	addCmdHelp("!stopsfx", "Stop a sound effect.", Tier::GUEST);

	addCmdHelp("!ban", "Ban a user.", Tier::MOD);
	addCmdHelp("!dc", "Disconnect a gamepad.", Tier::MOD);
	addCmdHelp("!decrease", "Decrease a user's hotseat time.", Tier::MOD);
	addCmdHelp("!extend", "Extend a user's hotseat time.", Tier::MOD);
	addCmdHelp("!warmup", "Reduce hotseat cooldown for a user.", Tier::MOD);
	addCmdHelp("!cooldown", "Increase hotseat cooldown for a user.", Tier::MOD);
	addCmdHelp("!kick", "Kick a user.", Tier::MOD);
	addCmdHelp("!lock", "Lock a gamepad.", Tier::MOD);
	addCmdHelp("!lockall", "Lock all gamepads.", Tier::MOD);
	addCmdHelp("!mute", "Mute a user.", Tier::MOD);
	addCmdHelp("!rc", "Reconnect a controller.", Tier::MOD);
	addCmdHelp("!restart", "Restart the game in kiosk mode.", Tier::MOD);
	addCmdHelp("!strip", "Strip a user's gamepad.", Tier::MOD);
	addCmdHelp("!stripall", "Strip all gamepads.", Tier::MOD);
	addCmdHelp("!unban", "Unban a user.", Tier::MOD);
	addCmdHelp("!unbanlastip", "Unban the last IP address.", Tier::MOD);
	addCmdHelp("!unmute", "Unmute a user.", Tier::MOD);
	addCmdHelp("!verify", "Verify a user.", Tier::MOD);
	addCmdHelp("!dcall", "Disconnect all gamepads.", Tier::MOD);
	addCmdHelp("!name", "Change the room name.", Tier::MOD);

	addCmdHelp("!addxbox", "Add an Xbox controller.", Tier::GOD);
	addCmdHelp("!addps", "Add a PS controller.", Tier::GOD);
	addCmdHelp("!private", "Set the game to private.", Tier::GOD);
	addCmdHelp("!public", "Set the game to public.", Tier::GOD);
	addCmdHelp("!speakers", "Set the volume.", Tier::GOD);
	addCmdHelp("!quit", "Quit hosting.", Tier::GOD);
	addCmdHelp("!mod", "Add a moderator.", Tier::GOD);
	addCmdHelp("!unmod", "Remove a moderator.", Tier::GOD);
	addCmdHelp("!unvip", "Remove a VIP.", Tier::GOD);
	addCmdHelp("!vip", "Add a VIP.", Tier::GOD);

	_chatBotCustom->addHelp();

	// Merge commands vector and _chatBotCustom commands vector
	if (_chatBotCustom->commands.size() > 0) {
		commands.insert(commands.end(), _chatBotCustom->commands.begin(), _chatBotCustom->commands.end());
	}

}

/*
Add a command's help/description.
*/
void ChatBot::addCmdHelp(std::string command, std::string desc, Tier tier) {
	commands.push_back(CommandInfo(command, desc, tier));
}

const uint32_t ChatBot::getLastUserId() const {
	return this->_lastUserId;
}

const std::string ChatBot::formatGuestConnection(Guest guest, ParsecGuestState state, ParsecStatus status) {
	setLastUserId(BOT_GUESTID);

	std::ostringstream reply;
	if (state == GUEST_CONNECTED)
	{
		reply << "@join \t\t " << guest.name << " #" << guest.userID << "\0";
	}
	else if (state == GUEST_FAILED)
	{
		reply << "!" << status << " \t\t " << guest.name << " #" << guest.userID << "\0";
	}
	else
	{
		switch (status)
		{
		case HOST_WRN_KICKED:
			reply << "!kick \t\t " << guest.name << " #" << guest.userID << "\0";
			break;
		case CONNECT_WRN_NO_ROOM:
			reply << "!full \t\t " << guest.name << " #" << guest.userID << "\0";
			break;
		case NETWORK_ERR_BG_TIMEOUT:
			reply << "!timeout \t\t " << guest.name << " #" << guest.userID << "\0";
			break;
		case SERVER_ERR_CLIENT_ABORT:
			reply << "!quit \t\t " << guest.name << " #" << guest.userID << "\0";
			break;
		default:
			reply << "!" << status << " \t\t " << guest.name << " #" << guest.userID << "\0";
			break;
		}
	}

	const std::string formattedMessage = reply.str();
	reply.clear();
	reply.flush();
	return formattedMessage;
}

const std::string ChatBot::formatBannedGuestMessage(Guest guest)
{
	std::ostringstream reply;
	reply << "!ban \t\t " << guest.name << " #" << guest.userID << "\0";

	return reply.str();
}

const std::string ChatBot::formatModGuestMessage(Guest guest) {

	std::ostringstream reply;
	reply << Config::cfg.chatbotName + "Moderator \n\t\t" << guest.name << " \t (#" << guest.userID << ") has joined!\0";

	return reply.str();

}

CommandBotMessage ChatBot::sendBotMessage(const char* msg, Guest& sender) {
	CommandBotMessage message(msg, sender);
	message.run();
	setLastUserId(BOT_GUESTID);
	return message;
}

std::vector<std::string> ChatBot::split(const std::string& str, char delimiter) {
	std::vector<std::string> tokens;
	std::stringstream ss(str);
	std::string token;

	while (std::getline(ss, token, delimiter)) {
		tokens.push_back(token);
	}

	return tokens;
}


bool ChatBot::isCommand(const char* msg, vector<const char*> patterns) {
	// Split the message by spaces
	vector<string> tokens = split(msg, ' ');

	// Does the first token match any of the patterns?
	vector<const char*>::iterator pi = patterns.begin();
	for (; pi != patterns.end(); ++pi) {
		if (tokens[0] == *pi) {
			return true;
		}
	}

	return false;
}

// Get the arguments of a command
vector<string> ChatBot::getArguments(const char* msg) {
	vector<string> tokens = split(msg, ' ');
	tokens.erase(tokens.begin());
	return tokens;
}

void ChatBot::setLastUserId(uint32_t lastId) {
	this->_lastUserId = lastId;
}

void ChatBot::updateSettings() {
	//_basicVersion = Config::cfg.general.basicVersion;
}


