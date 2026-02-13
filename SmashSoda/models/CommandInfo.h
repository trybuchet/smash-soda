#pragma once
#include <string>
#include "../Tier.h"

class CommandInfo {
public:
	std::string command;
	std::string desc;
	Tier tier;

	CommandInfo(std::string command, std::string desc, Tier tier) {
		this->command = command;
		this->desc = desc;
		this->tier = tier;
	}
};


