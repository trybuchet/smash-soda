#pragma once

#include <string>
#include <sstream>
#include <vector>
#include <regex>
#include "parsec-dso.h"
#include "../ParsecSession.h"
#include "Stringer.h"
#include "../core/GamepadClient.h"
#include "../DX11.h"
#include "../AudioIn.h"
#include "../Core/Cache.h"
#include "../Models/CommandInfo.h"

#include "ACommand.h"
#include "Commands/CommandBotMessage.h"
#include "Commands/CommandDefaultMessage.h"

#include "Commands/God/CommandAddXbox.h"
#include "Commands/God/CommandAddPS.h"
#include "Commands/God/CommandMod.h"
#include "Commands/God/CommandPrivate.h"
#include "Commands/God/CommandPublic.h"
#include "Commands/God/CommandQuit.h"
#include "Commands/God/CommandSpeakers.h"
#include "Commands/God/CommandUnmod.h"
#include "Commands/God/CommandVersion.h"
#include "Commands/God/CommandVIP.h"
#include "Commands/God/CommandUnVIP.h"

#include "Commands/Mod/CommandBan.h"
#include "Commands/Mod/CommandDecrease.h"
#include "Commands/Mod/CommandCooldown.h"
#include "Commands/Mod/CommandDC.h"
#include "Commands/Mod/CommandExtend.h"
#include "Commands/Mod/CommandWarmup.h"
#include "Commands/Mod/CommandHotseat.h"
#include "Commands/Mod/CommandKick.h"
#include "Commands/Mod/CommandMute.h"
#include "Commands/Mod/CommandLock.h"
#include "Commands/Mod/CommandLockAll.h"
#include "Commands/Mod/CommandOne.h"
#include "Commands/Mod/CommandName.h"
#include "Commands/Mod/CommandRC.h"
#include "Commands/Mod/CommandRestart.h"
#include "Commands/Mod/CommandStrip.h"
#include "Commands/Mod/CommandStripAll.h"
#include "Commands/Mod/CommandUnban.h"
#include "Commands/Mod/CommandUnmute.h"
#include "Commands/Mod/CommandVideoFix.h"
#include "Commands/Mod/CommandVerify.h"
#include "Commands/Mod/CommandDCAll.h"
#include "Commands/Mod/CommandUnbanLastIP.h"

#include "Commands/Guest/Command8ball.h"
#include "Commands/Guest/CommandBB.h"
#include "Commands/Guest/CommandBonk.h"
#include "Commands/Guest/CommandDiscord.h"
#include "Commands/Guest/CommandFF.h"
#include "Commands/Guest/CommandFortune.h"
#include "Commands/Guest/CommandHelp.h"
#include "Commands/Guest/CommandKeyboard.h"
#include "Commands/Guest/CommandMirror.h"
#include "Commands/Guest/CommandPads.h"
#include "Commands/Guest/CommandPing.h"
#include "Commands/Guest/CommandRollCall.h"
#include "Commands/Guest/CommandRPG.h"
#include "Commands/Guest/CommandSFX.h"
#include "Commands/Guest/CommandStopSFX.h"
#include "Commands/Guest/CommandSpectate.h"
#include "Commands/Guest/CommandSwap.h"
#include "Commands/Guest/CommandTriangle.h"

#include "../CompilerDirectives.h"
#include "Macro.h"
#include "Hotseat.h"

#include "ChatBotCustom.h"

#define BOT_GUESTID 0

class ChatBot
{
public:

	ChatBot(
		AudioIn& audioIn, AudioOut& audioOut, DX11& dx11,
		GuestList& guests, GuestDataList& guestHistory, ParsecDSO* parsec, ParsecHostConfig& hostConfig,
		ParsecSession& parsecSession, Macro& macro, bool& hostingLoopController, Guest& host,
		Hotseat& hotseat
	)
		: _audioIn(audioIn), _audioOut(audioOut), _dx11(dx11),
		_guests(guests), _guestHistory(guestHistory), _parsec(parsec), _hostConfig(hostConfig), 
		_parsecSession(parsecSession), _macro(macro), _hostingLoopController(hostingLoopController), _host(host),
		_hotseat(hotseat)
	{
		_chatBotCustom = new ChatBotCustom(audioIn, audioOut, dx11, guests, guestHistory, parsec, hostConfig, parsecSession, macro, hostingLoopController, host, hotseat);
		addHelp();
	}

	ACommand * identifyUserDataMessage(const char* msg, Guest& sender, bool isHost = false);

	const uint32_t getLastUserId() const;
	void setLastUserId(const uint32_t lastId = BOT_GUESTID);

	const std::string formatGuestConnection(Guest guest, ParsecGuestState state, ParsecStatus status);
	const std::string formatBannedGuestMessage(Guest guest);
	const std::string formatModGuestMessage(Guest guest);
	CommandBotMessage sendBotMessage(const char * msg, Guest& sender);
	void updateSettings();

	std::vector<std::string> split(const std::string& str, char delimiter);
	std::vector<std::string> getArguments(const char* msg);
	bool isCommand(const char* msg, vector<const char*> patterns);

	std::vector<CommandInfo> commands = {};
	void addHelp();
	void addCmdHelp(std::string command, std::string desc, Tier tier);

private:
	static bool msgStartsWith(const char* msg, const char* pattern);
	static bool msgStartsWith(const char* msg, vector<const char*> patterns);
	static bool msgIsEqual(const char* msg, const char* pattern);
	static bool msgIsEqual(const char* msg, vector<const char*> patterns);

	uint32_t _lastUserId = 0;

	// Dependency Injection
	ParsecDSO* _parsec;
	AudioIn& _audioIn;
	AudioOut& _audioOut;
	DX11 &_dx11;
	GuestList& _guests;
	GuestDataList& _guestHistory;
	ParsecHostConfig &_hostConfig;
	ParsecSession &_parsecSession;
	Macro& _macro;
	Hotseat& _hotseat;
	bool &_hostingLoopController;
	Guest& _host;
	ChatBotCustom* _chatBotCustom;
	bool _basicVersion = false;
};


