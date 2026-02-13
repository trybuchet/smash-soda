#pragma once

#include <iostream>
#include <vector>
#include <functional>
#include "matoya.h"
#include "PathHelper.h"
#include "../Models/GuestData.h"
#include "../GuestDataList.h"

class VIPList : GuestDataList
{
public:
	VIPList();
	VIPList(const std::vector<GuestData> vipUsers);
	bool VIP(const GuestData user);
	const bool unVIP(const uint32_t userID);
	const bool unVIP(string guestName);
	const bool isVIP(const uint32_t userID);
	vector<GuestData>& getGuests();
	static vector<GuestData> LoadFromFile();
	bool SaveToFile();
};


