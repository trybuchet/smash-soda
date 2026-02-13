#pragma once

#include <vector>
#include <sstream>
#include "parsec.h"
#include "Guest.h"
#include "Stringer.h"
#include <map>
#include <algorithm>
#include <random>

using namespace std;

#define GUESTLIST_MAX_GUESTS 64

typedef struct MyMetrics
{
	ParsecMetrics metrics{ 0, 0, 0, 0, 0.0, 0.0, 0.0, 0.0 };
	int congested{ 0 };
	float averageNetworkLatency{ 0 };
	uint32_t averageNetworkLatencySize{ 0 };
} MyMetrics;

class GuestList
{
public:
	static GuestList& instance;

	void setGuests(ParsecGuest* guests, int guestCount);
	vector<Guest> &getGuests();
	vector<Guest> getPlayingGuests();
	vector<Guest> getRandomGuests(int count = 1, bool ignoreSpectators = true);
	vector<Guest> getGuestsAfterGuest(uint32_t targetGuestID, int count = 1, bool ignoreSpectators = true);
	
	void toggleSpectator(uint32_t targetGuestID);

	void clear();
	const bool find(uint32_t targetGuestID, Guest *result);
	const bool find(const char* targetName, Guest* result);
	const bool find(string targetName, Guest* result);
	const int findIndex(uint32_t targetGuestID);
	MyMetrics getMetrics(uint32_t id);
	void deleteMetrics(uint32_t id);
	void updateMetrics(ParsecGuest* guests, int guestCount);
	bool pop(uint32_t userID);
	bool setInputPermissions(uint32_t userID, bool gamepad, bool keyboard, bool mouse);

	const char* guestNames[GUESTLIST_MAX_GUESTS];
private:
	vector<Guest> _guests;
	map<uint32_t, MyMetrics> _metrics;
	map<uint32_t, ParsecPermissions> _inputPermissions;
};



