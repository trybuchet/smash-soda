#pragma once

#include <vector>
#include "../GuestTier.h"

class TierList
{
public:
	void setTier(uint32_t userID, Tier tier);
	Tier getTier(uint32_t userID);
	
	bool saveTiers();
	void loadTiers();
	
private:
	vector<GuestTier> _guestTiers;
};


