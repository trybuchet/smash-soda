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
#include "../AudioOut.h"
#include "../Core/Cache.h"
#include "../CompilerDirectives.h"
#include "Macro.h"
#include "Hotseat.h"
#include "../Models/CommandInfo.h"

#include "ACommand.h"
#include "commands/CommandBotMessage.h"
#include "commands/CommandDefaultMessage.h"

class ChatBotCustom
{
public:
	ChatBotCustom(
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
		//_basicVersion = Config::cfg.general.basicVersion;
	}
	ACommand* isCustomCommand(const char* msg, Guest& sender, bool isHost, Tier tier, uint32_t previous);

	std::vector<CommandInfo> commands = {};
	std::vector<std::string> split(const std::string& str, char delimiter);
	std::vector<std::string> getArguments(const char* msg);
	bool isCommand(const char* msg, vector<const char*> patterns);
	void addHelp();
	void addCmdHelp(std::string command, std::string desc, Tier tier);
private:
	// Dependency Injection
	ParsecDSO* _parsec;
	AudioIn& _audioIn;
	AudioOut& _audioOut;
	DX11& _dx11;
	GuestList& _guests;
	GuestDataList& _guestHistory;
	ParsecHostConfig& _hostConfig;
	ParsecSession& _parsecSession;
	Macro& _macro;
	Hotseat& _hotseat;
	bool& _hostingLoopController;
	Guest& _host;

	bool msgStartsWith(const char* msg, const char* pattern);
	bool msgStartsWith(const char* msg, vector<const char*> patterns);
	bool msgIsEqual(const char* msg, const char* pattern);
	bool msgIsEqual(const char* msg, vector<const char*> patterns);
};


