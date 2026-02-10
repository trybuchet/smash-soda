#pragma once

#include "../../../Core/Config.h"
#include <string>
#include <unordered_map>
#include "ACommand.h"
#include "../../../core/GuestList.h"

using namespace std;

/// <summary>
/// Hail and well met, traveler! This is a stupid little RPG
/// game I made (with help from ChatGPT for generating some sentences), 
/// that you can easily customize to your liking, and hopefully serve
/// as a good example on how to make your own commands. Excelsior!
/// - Mickey
/// 
///          />_________________________________
/// [########[]_________________________________ >
///          \>
/// </summary>
class CommandRPG : public ACommand {
public:

	/**
	 * @brief Construct a new CommandRPG object
	 * 
	 * @param msg
	 * @param sender
	 * @param guestList
	 */
	CommandRPG(const char* msg, Guest& sender, GuestList& guestList)
	: ACommand(msg, sender), _guestList(guestList) {}

	/**
	 * @brief Run the command
	 * @return true if the command was successful
	 */
	bool run() override {

		isBotCommand = true;

		// Are we making an RPG sentence, opening a chest, or having a battle?
		int choice = rand() % 3;

		// Are there enough players to have a battle?
		if (choice == 2 && _guestList.getGuests().size() < 2) {
			choice = rand() % 2;
		}

		// Handle the RPG sentence.
		switch (choice) {
			// RPG sentence.
			case 0: {
				// Pick a random sentence.
				std::string sentence = _rpgSentences[rand() % _rpgSentences.size()];

				// Pick a random monster name.
				std::string monster = _monsterNames[rand() % _monsterNames.size()];

				// Replace the placeholders with the player's name and the monster's name.
				std::unordered_map<std::string, std::string> placeholders = {
					{"0", _sender.name},
					{"1", monster}
				};
				_replyMessage = Config::cfg.chatbotName + replacePlaceholders(sentence, placeholders);
				break;
			}
			// Chest sentence.
			case 1: {
				// Pick a random sentence.
				std::string sentence = _chestSentences[rand() % _chestSentences.size()];

				// Replace the placeholder with the player's name.
				std::unordered_map<std::string, std::string> placeholders = {
					{"0", _sender.name}
				};
				_replyMessage = Config::cfg.chatbotName + replacePlaceholders(sentence, placeholders);
				break;
			}
			// Battle sentence.
			case 2: {
				// Pick a random sentence.
				std::string sentence = _battleSentences[rand() % _battleSentences.size()];

				// Pick a random player to battle.
				Guest& opponent = _guestList.getGuests()[rand() % _guestList.getGuests().size()];

				// If opponent matches sender, pick a random warrior name.
				if (opponent.name == _sender.name) {
					opponent.name = _warriorNames[rand() % _warriorNames.size()];
				}

				// Pick a random winner.
				Guest& winner = rand() % 2 == 0 ? _sender : opponent;

				// Replace the placeholders with the player's names.
				std::unordered_map<std::string, std::string> placeholders = {
					{"0", _sender.name},
					{"1", opponent.name},
					{"2", winner.name}
				};
				_replyMessage = Config::cfg.chatbotName + replacePlaceholders(sentence, placeholders);
				break;
			}
		}
		
		return true;

	}

	/**
	 * @brief Get the prefixes object
	 * @return vector<const char*> 
	 */
	static vector<const char*> prefixes() {
		return vector<const char*> { "!rpg" };
	}

protected:

	GuestList& _guestList;

	/**
	 * @brief RPG sentences to use in the command.
	 */
	vector<std::string> _rpgSentences = {
		"{0} entered the dungeon and encountered a ferocious {1}. After a long and hard-fought battle, {0} emerged victorious!",
		"As {0} ventured into the forest, they stumbled upon a pack of hungry {1}s. {0} bravely fought them off, earning the respect of the woodland creatures.",
		"{0} explored the abandoned castle and encountered a sinister {1}. With quick reflexes and sharp wit, {0} defeated the dark sorcerer.",
		"In the depths of the cavern, {0} faced off against a {1}. Despite its venomous bite, {0} emerged triumphant.",
		"As the mist cleared in the graveyard, {0} found themselves face to face with a vengeful {1}. With nerves of steel, {0} banished the spirit to the afterlife.",
		"Venturing into the haunted mansion, {0} encountered a horde of restless {1}s. Through sheer determination, {0} managed to lay them to rest.",
		"{0} bravely entered the coliseum, where they were met by a fierce {1}. With strength and strategy, {0} defeated the mighty beast in single combat.",
		"Amidst the ruins of an ancient temple, {0} faced a cunning {1}. Through riddles and combat, {0} proved their worth and earned passage to the treasure within.",
		"{0} journeyed into the icy tundra and stumbled upon a fearsome {1}. Despite the freezing cold, {0} emerged victorious in the struggle.",
		"Deep within the forest, {0} confronted a legendary {1}. With blade in hand, {0} managed to slay the beast and end its reign of terror.",
		"{0} entered the dungeon and encountered a clumsy {1} who kept tripping over its own feet. After a hilariously inept battle, {0} emerged victorious!",
		"As {0} ventured into the forest, they stumbled upon a group of squirrels performing an elaborate dance routine. Unable to resist joining in, {0} danced their way to victory.",
		"{0} explored the abandoned castle and found themselves in a heated debate with a sentient suit of armor over the proper way to polish metal. In the end, {0} won the argument and the respect of the armor.",
		"In the depths of the cavern, {0} faced off against a giant {1} with an identity crisis, unsure whether it wanted to be jelly or gelatin. Despite its existential crisis, {0} emerged triumphant.",
		"As the mist cleared in the graveyard, {0} found themselves face to face with a group of ghostly musicians jamming out some hauntingly catchy tunes. Unable to resist the groove, {0} joined in and rocked the spirits into submission.",
		"Venturing into the haunted mansion, {0} encountered a ghost who was more interested in sharing ghost stories than fighting. After a spooky storytelling session, {0} earned the ghost's friendship and a key to the treasure room.",
		"{0} bravely entered the coliseum, where they were met by a chicken wearing a gladiator helmet. Despite the poultry opponent's fierce clucks, {0} emerged victorious in a battle that would go down in history as the 'Great Chicken Showdown.'",
		"Amidst the ruins of an ancient temple, {0} faced a cunning {1} who had mistaken the temple's treasures for its hidden acorn stash. Through negotiations involving nuts and shiny objects, {0} managed to retrieve the treasures without resorting to violence.",
		"{0} journeyed into the icy tundra and stumbled upon a snowman who was determined to prove its worth as a fearsome guardian. After a snowball fight for the ages, {0} emerged victorious, leaving the snowman in a puddle of defeat.",
		"Deep within the forest, {0} confronted a legendary unicorn who was more interested in discussing the latest magical fashion trends than fighting. After a fashion show-off that would make even the most stylish wizards jealous, {0} earned the unicorn's respect and a glittery hoof bump of victory."
	};

	/**
	 * @brief Chest sentences to use in the command.
	 */
	vector<std::string> _chestSentences = {
		"{0} opened a chest and found a gleaming sword, a powerful weapon to aid them in their adventures!",
		"Inside the chest, {0} discovered a dusty tome filled with ancient spells, granting them newfound magical abilities.",
		"As {0} opened the chest, a cloud of noxious gas billowed out, leaving them coughing and spluttering.",
		"{0} eagerly opened the chest, only to find it empty. It seems someone had beaten them to the treasure.",
		"Within the chest, {0} found a map leading to a hidden treasure trove, promising riches beyond imagination.",
		"The chest contained a mysterious potion, its effects unknown to {0}. They decided to keep it for later use.",
		"{0} cautiously opened the chest, revealing a swarm of angry bees. They hastily closed it before making their escape.",
		"A burst of confetti erupted from the chest as {0} opened it, accompanied by a note congratulating them on their find.",
		"Inside the chest, {0} found a cursed artifact that seemed to pulse with dark energy. They quickly disposed of it before any harm could befall them.",
		"The chest contained a pile of gold coins, a welcome reward for {0}'s daring exploits.",
		"{0} opened a chest and found a pair of enchanted socks. Despite their questionable fashion sense, they provide surprisingly warm feet.",
		"Inside the chest, {0} discovered a rubber chicken with a note attached: 'For emergency comedic relief.'",
		"As {0} eagerly opened the chest, a small but enthusiastic party of gnomes burst out, cheering and offering celebratory dances.",
		"{0} cautiously opened the chest, only to be greeted by a swarm of mischievous fairies who proceed to redecorate their surroundings with glitter.",
		"Within the chest, {0} found a jar of pickles. While not the treasure they were expecting, it certainly made for an interesting snack.",
		"The chest contained a collection of mismatched socks, each with a note attached detailing its mysterious disappearance from the laundry.",
		"As {0} opened the chest, a troupe of singing mice emerged, performing a catchy tune that stuck in their head for days.",
		"{0} found a talking parrot inside the chest, who proceeded to engage them in a spirited debate about the merits of pirate life versus adventuring.",
		"Inside the chest, {0} discovered a pair of enchanted underpants. While they may not offer much in terms of protection, they certainly provide a confidence boost.",
		"The chest contained a map leading to another chest, which in turn contained a map leading back to the original chest. It seems the real treasure was the adventure all along.",
		"{0} opened a chest and found a majestic pair of diapers. Nobody tell Mar-Tea.",
	};

	/**
	 * @brief Battle sentences to use in the command.
	 */
	vector<std::string> _battleSentences = {
		"{0} clashed swords with {1} in a fierce duel. In the end, {2} emerged victorious!",
		"{0} and {1} engaged in a heated battle of wits and strength. The outcome was uncertain until {2} finally gained the upper hand.",
		"As {0} and {1} squared off, the tension in the air was palpable. In the end, it was {2} who emerged triumphant.",
		"The battle between {0} and {1} raged on, each combatant refusing to yield. However, it was {2} who ultimately claimed victory.",
		"{0} and {1} clashed in an epic showdown, their skills evenly matched. In the end, luck favored {2} as they emerged as the winner.",
		"The arena erupted into cheers as {0} and {1} faced off in a battle for glory. In the end, it was {2} who stood triumphant.",
		"{0} and {1} engaged in a fierce magical duel, hurling spells and incantations at each other. In the end, it was {2} who emerged victorious.",
		"The battle between {0} and {1} raged on, each blow struck with deadly precision. When the dust settled, it was {2} who stood as the last one standing.",
		"As {0} and {1} clashed on the battlefield, the earth shook with the force of their blows. In the end, it was {2} who emerged as the stronger warrior.",
		"In a showdown for the ages, {0} and {1} faced off in a battle of champions. When the dust settled, it was {2} who emerged victorious.",
		"{0} and {1} engaged in a fierce pillow fight, with feathers flying everywhere. In the end, it was {2} who emerged victorious, crowned the Pillow King/Queen of the realm!",
		"As {0} and {1} faced off, they were suddenly transformed into chickens by a misfired spell. After a hilarious battle of pecks and squawks, {2} emerged as the 'Cock of the Walk.'",
		"The battle between {0} and {1} turned into a heated argument about the superiority of cats versus dogs. In the end, it was {2} who convinced everyone that both were equally adorable.",
		"In a surprising turn of events, {0} and {1} challenged each other to a baking contest instead of a battle. After a series of delicious treats and culinary mishaps, {2} emerged as the undisputed 'Bake-Off Champion.'",
		"{0} and {1} decided to settle their differences with a dance-off instead of a battle. After a series of outrageous dance moves and embarrassing twirls, {2} emerged as the 'Dancing Dynamo' of the party.",
		"As {0} and {1} prepared to battle, they were interrupted by a group of friendly kittens who just wanted to play. After a chaotic but adorable skirmish, {2} emerged as the 'Kitty Cuddler' of the group.",
		"{0} and {1} challenged each other to a staring contest instead of a battle. After an intense gaze-off that lasted for hours, {2} blinked first, earning the title of 'Master of the Stare.'",
		"The battle between {0} and {1} turned into a pun-off, with each combatant trying to outwit the other with their wordplay. In the end, it was {2} who delivered the pun that cracked everyone up.",
		"As {0} and {1} squared off, they were suddenly interrupted by a group of friendly woodland creatures who just wanted to hug it out. After a heartwarming embrace, {2} emerged as the 'Cuddle Champion' of the forest.",
		"In a bizarre turn of events, {0} and {1} challenged each other to a game of rock-paper-scissors instead of a battle. After a series of intense throws and dramatic reveals, {2} emerged as the 'Rock-Paper-Scissors Master' of the realm."
	};

	/**
	 * @brief Random monster names to use in battles.
	 */
	vector<std::string> _monsterNames = {
		"dragon",
		"troll",
		"ogre",
		"werewolf",
		"vampire",
		"zombie",
		"ghost",
		"witch",
		"necromancer",
		"minotaur",
		"goblin",
		"specter",
		"harpy",
		"centaur",
		"kraken",
		"cyclops",
		"siren",
		"basilisk",
		"chimera",
		"manticore"
	};

	/**
	 * @brief Random warrior names to use in battles.
	 */
	vector<std::string> _warriorNames = {
		"Thorn",
		"Raven",
		"Shadow",
		"Blade",
		"Storm",
		"Nightshade",
		"Scarlet",
		"Viper",
		"Phoenix"
	};

	/**
	 * @brief Replace placeholders in a string with values from a map.
	 * 
	 * @param str The string to replace placeholders in.
	 * @param placeholders The map of placeholders and their values.
	 * @return The string with placeholders replaced.
	 */
	std::string replacePlaceholders(const std::string& str, const std::unordered_map<std::string, std::string>& placeholders) {
		std::string result = str;
		for (const auto& pair : placeholders) {
			std::string placeholder = "{" + pair.first + "}";
			size_t pos = result.find(placeholder);
			while (pos != std::string::npos) {
				result.replace(pos, placeholder.length(), pair.second);
				pos = result.find(placeholder);
			}
		}
		return result;
	}

	/**
	 * @brief Get the prefixes object
	 * @return vector<const char*> 
	 */
	static vector<const char*> internalPrefixes() {
		return vector<const char*> { "!rpg " };
	}
};


