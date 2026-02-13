#pragma once

#include <iostream>
#include <vector>
#include <functional>
#include "matoya.h"
#include "../Models/GuestData.h"
#include "../GuestDataList.h"
#include "PathHelper.h"

class BanList : GuestDataList
{
public:
	BanList();
	BanList(const std::vector<GuestData> bannedUsers);
	bool ban (const GuestData user);
	const bool unban(const uint32_t userID);
	const bool unban(string guestName);
	const bool isBanned(const uint32_t userID);
	const bool isBanned(string guestName);	
	vector<GuestData>& getGuests();
	static vector<GuestData> LoadFromFile();
	bool SaveToFile();
};




