#include "ChatBotCustom.h"

/*
* Include all your custom commands here.
* 
* Changes to this file and files inside Commands/Custom
* folder will be ignored by git, to avoid conflicts when
* pulling updates from the repository.
*/

//#include "Commands/Custom/YourCommandClass.h"

/**
* This function is where you check to see if the
* message matches your command prefixes.
**/
ACommand* ChatBotCustom::isCustomCommand(const char* msg, Guest& sender, bool isHost, Tier tier, uint32_t previous) {

	/*if (isCommand(msg, YourCommandClass::prefixes())) {
		return new YourCommandClass(msg, sender, _parsec, _guests, _guestHistory);
	}*/

	// Returns a default message if no custom command is found, so the bot can still respond.
	return new CommandDefaultMessage(msg, sender, previous, tier, isHost);
}

/*
Add all the help/descriptions for the commands.
*/
void ChatBotCustom::addHelp() {

	//addCmdHelp("!yourcommand", "Description", Tier::GUEST);

}

/**
* Don't edit anything below this line. Use these functions
* to check if a message starts with a pattern or is equal to
* a pattern.
**/

bool ChatBotCustom::msgStartsWith(const char* msg, const char* pattern) {
	return Stringer::startsWithPattern(msg, pattern);
}

bool ChatBotCustom::msgStartsWith(const char* msg, vector<const char*> patterns) {
	vector<const char*>::iterator pi = patterns.begin();
	for (; pi != patterns.end(); ++pi)
	{
		if (Stringer::startsWithPattern(msg, *pi))
		{
			return true;
		}
	}
	return false;
}

bool ChatBotCustom::msgIsEqual(const char* msg, const char* pattern) {
	return (strcmp(msg, pattern) == 0);
}

bool ChatBotCustom::msgIsEqual(const char* msg, vector<const char*> patterns) {
	vector<const char*>::iterator pi = patterns.begin();
	for (; pi != patterns.end(); ++pi)
	{
		if ((strcmp(msg, *pi) == 0))
		{
			return true;
		}
	}
	return false;
}

std::vector<std::string> ChatBotCustom::split(const std::string& str, char delimiter) {
	std::vector<std::string> tokens;
	std::stringstream ss(str);
	std::string token;

	while (std::getline(ss, token, delimiter)) {
		tokens.push_back(token);
	}

	return tokens;
}


bool ChatBotCustom::isCommand(const char* msg, vector<const char*> patterns) {
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
vector<string> ChatBotCustom::getArguments(const char* msg) {
	vector<string> tokens = split(msg, ' ');
	tokens.erase(tokens.begin());
	return tokens;
}

/*
Add a command's help/description.
*/
void ChatBotCustom::addCmdHelp(std::string command, std::string desc, Tier tier) {
	commands.push_back(CommandInfo(command, desc, tier));
}


