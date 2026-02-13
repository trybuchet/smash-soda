#pragma once

#include <vector>
#include <thread>
#include <direct.h>
#include <iostream>
#include <windows.h>
#include <shlobj.h>
#include <chrono>
#include <ctime>
#include "Stopwatch.h"
#include "../Models/GuestData.h"

/// <summary>
/// Handles the hotseat system.
/// </summary>
class AutoMod {
public:

	class MutedUser {
	public:
		int userId = 0;
		string userName = "";
		Stopwatch* stopwatch = new Stopwatch();
	};

	class ChatMessage {
	public:
		int userId = 0;
		string userName = "";
		string message = "";
	};

	vector<MutedUser> mutedUsers;
	vector<ChatMessage> lastMessages;

	AutoMod();

	void Start();
	void Stop();

	void RecordMessage(int userId, string userName, string message);

	void MuteUser(int userId, string userName, int duration);
	void UnmuteUser(int userId);
	bool isMuted(int userId);

	void Log(string message);

	static AutoMod instance;

private:
	thread _thread;
	bool isRunning = false;

	MutedUser* GetMutedUser(int userId);
};


