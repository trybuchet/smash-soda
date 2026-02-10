#pragma once

#include "ACommand.h"
#include "../../../Guest.h"

using namespace std;

class CommandHelp : public ACommand
{
public:

	/**
	 * @brief Construct a new CommandHelp object
	 * 
	 * @param sender 
	 */
	CommandHelp(const char* msg, Guest& sender, vector<CommandInfo> commandList)
		: ACommand(msg, sender), sender(sender), commands(commandList)
	{}

	/**
	 * @brief Run the command
	 * @return true if the command was successful
	 */
	bool run() override {
		//_replyMessage = Config::cfg.chatbotName + "For a full list of commands, visit: https://bit.ly/smash-soda";

		vector<CommandInfo> filteredCommands;
		vector<CommandInfo> pageCommands;
		size_t page = 0;
		if (getArgs().size() > 0) {
			try {
				page = stoi(getArgs()[0]);
			}
			catch (const std::exception&) {
				_replyMessage = "Invalid page number.";
				return false;
			}
		}

		// Filter commands based on tier
		Tier tier = Cache::cache.tierList.getTier(sender.userID);
		for (CommandInfo& cmd : commands) {
			if (cmd.tier <= tier) {
				filteredCommands.push_back(cmd);
			}
		}

		// Check if page is out of bounds
		if (page < 1) {
			page = 1;
		}
		else if (page > (filteredCommands.size() + perPage - 1) / perPage) {
			page = (filteredCommands.size() + perPage - 1) / perPage;
		}

		// Calculate total pages (round up)
		size_t totalPages = (filteredCommands.size() + perPage - 1) / perPage;

		// Get commands for this page
		for (size_t i = (page - 1) * perPage; i < page * perPage && i < filteredCommands.size(); i++) {
			pageCommands.push_back(filteredCommands[i]);
		}

		std::string help = "```\n";
		for (CommandInfo& cmd : pageCommands) {
			help += cmd.command + " - " + cmd.desc + "\n";
		}
		help += "``` PAGE " + to_string(page) + "/" + to_string(totalPages) + " ```";
		_replyMessage = help;

		return true;
	}

	/**
	 * @brief Get the prefixes object
	 * @return vector<const char*> 
	 */
	static vector<const char*> prefixes() {
		return vector<const char*> { "!help", "!commands" };
	}

protected:
	vector<CommandInfo> commands;
	Guest& sender;
	int perPage = 10;
};




