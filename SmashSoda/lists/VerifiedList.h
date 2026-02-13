#pragma once

#include <iostream>
#include <vector>
#include <functional>
#include "matoya.h"
#include "../Models/GuestData.h"
#include "../GuestDataList.h"
#include "PathHelper.h"

class VerifiedList : GuestDataList {
public:
	VerifiedList();
	VerifiedList(const std::vector<GuestData> verifiedUsers);
	bool Add(const GuestData user);
	bool Verify(const GuestData user);
	const bool Remove(const uint32_t userID);
	const bool Remove(const string userName);
	const bool inList(const uint32_t userID);
	vector<GuestData>& getGuests();
	static vector<GuestData> LoadFromFile();
private:
	bool SaveToFile();
};


