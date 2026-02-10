#pragma once

#include <vector>
#include <thread>
#include <direct.h>
#include <iostream>
#include <windows.h>
#include <shlobj.h>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <ctime>
#include "StopwatchTimer.h"
#include "../Models/GuestData.h"

#include <nlohmann/json.hpp>
using namespace std;
using namespace std::chrono;
using json = nlohmann::json;

/// <summary>
/// Handles the hotseat system.
/// </summary>
class Hotseat {
public:

	class HotseatUser {
	public:
		bool cooldown = false; // Is the user waiting for their next turn?
		int userId = 0; // Parsec ID	
		string userName = ""; // Parsec username

		bool inSeat = false; // Is the user currently in the hotseat?
		StopwatchTimer* stopwatch; // Stopwatch timer for the user's playtime
		StopwatchTimer* cooldownTimer; // Stopwatch timer for the user's cooldown time
		time_t timeLastPlayed = 0; // The last time the user played
	};

	vector<HotseatUser> users; // List of user hotseat data
	
	Hotseat();

	void start();
	void stop();

	HotseatUser* getUser(int id);

	bool checkUser(int id, string name);
	void removeUser(int id);
	bool seatUser(int id, string name);
	void pauseUser(int id);
	void extendUserPlaytime(int id, long ms);
	void reduceUserPlaytime(int id, long ms);
	void increaseUserCooldown(int id, long ms);
	void decreaseUserCooldown(int id, long ms);

	bool inSeat(int id);

	string getUserTimeRemaining(int id);
	string getCooldownRemaining(int id);

	void log(string message);

	static Hotseat instance; // Singleton instance
	bool updated = false; // Has the hotseat data been updated?
	bool running = false; // Is the hotseat system running?

private:
	thread hotseatThread; // Hotseat thread for checking user playtime
	StopwatchTimer* reminderTimer; // Timer for reminder interval

	void _createUser(int id, string name);

	time_t getCurrentTimestamp();
	std::time_t addMinutesToTimestamp(std::time_t timestamp, int minutesToAdd);
	std::time_t subtractMinutesFromTimestamp(std::time_t timestamp, int minutesToSubtract);
	int getMinutesDifference(std::time_t timestamp1, std::time_t timestamp2);
	
};


