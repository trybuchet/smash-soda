#pragma once

#include <iostream>

using namespace std;

/**
 *  A reduced data structure to hold basic guest data.
 */
class GameData {
public:

	/**
	 *  Empty constructor.
	 */
	GameData() {
		itemID = 0;
		name = "";
		path = "";
		parameters = "";
	}

	/**
	 * Full constructor.
	 */
	GameData(uint32_t itemID, string name, string path, string param) {
		set(itemID, name, path, param);
	}


	void set(uint32_t itemID, string name, string path, string param) {
		this->itemID = itemID;
		this->name = name;
		this->path = path;
		this->parameters = param;
	}

	uint32_t itemID;
	std::string name;
	std::string path;
	std::string parameters;

};




