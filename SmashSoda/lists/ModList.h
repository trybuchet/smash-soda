#pragma once

#include <iostream>
#include <vector>
#include <functional>
#include "../Models/GuestData.h"
#include "../GuestDataList.h"
#include "PathHelper.h"

class ModList : GuestDataList {

public:
	ModList();
	ModList(const std::vector<GuestData> moddedUsers);

	bool mod(const GuestData user);
	const bool unmod(const uint32_t userID);
	const bool unmod(string guestName);
	const bool isModded(const uint32_t userID);
	vector<GuestData>& getGuests();
	static vector<GuestData> LoadFromFile();
private:
	bool SaveToFile();

};


