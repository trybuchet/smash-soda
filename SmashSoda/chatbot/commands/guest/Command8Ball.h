#pragma once

#include "ACommand.h"
#include <iostream>
#include "../../../Helpers/Dice.h"

using namespace std;

/**
 * @brief Command to get a random 8ball response
 */
class Command8Ball : public ACommand {
public:

	/**
	 * Example of how to use the command
	 */
	std::string usage = "!8ball <question>\nExample: !8ball Will I win at smash?";

	/**
	 * @brief Construct a new Command8Ball object
	 *
	 * @param sender
	 */
	Command8Ball(const char* msg, Guest& sender)
		: ACommand(msg, sender), _sender(sender)
	{}

	/**
	 * @brief Run the command
	 * @return true if the command was successful
	 */
	bool run() override {

		int seed = static_cast<unsigned int>(time(NULL));
		srand(seed);

		// Check if the user has provided a question
		if (getArgs().size() < 1) {
			setReply(usage);
			return false;
		}

		// Randomly select a category (0 for yes, 1 for maybe, 2 for no)
		int category = rand() % 3;

		// Randomly select a response from the chosen category
		int responseIndex = rand() % 10;

		// Ensure we don't pick an empty string for maybe and no categories
		while (responses[category][responseIndex].empty()) {
			responseIndex = rand() % 10;
		}

		setReply(responses[category][responseIndex] + ", " + _sender.name + ".");

		return true;

	}

	/**
	 * @brief Get the prefixes object
	 * @return vector<const char*>
	 */
	static vector<const char*> prefixes() {
		return vector<const char*> { "!8ball" };
	}

protected:
	Guest& _sender;

	string responses[3][10] = {
		{
			"As I see it, yes",
			"It is certain",
			"It is decidedly so",
			"Most likely",
			"Outlook good",
			"Signs point to yes",
			"Without a doubt",
			"Yes",
			"Yes - definitely",
			"You may rely on it"
		},
		{
			"Reply hazy, try again",
			"Ask again later",
			"Better not tell you now",
			"Cannot predict now",
			"Concentrate and ask again",
			"Maybe",
			"Dunno",
			"Beats me",
			"Stranger things have happened",
			"Who knows"
		},
		{
			"Don't count on it",
			"My reply is no",
			"My sources say no",
			"Outlook not so good",
			"Very doubtful",
			"Unlikely",
			"Absolutely not",
			"Definitely not",
			"No",
			"lol nope"
		}
	};
};


