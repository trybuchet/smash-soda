#pragma once

#include <vector>
#include <thread>
#include <direct.h>
#include <iostream>
#include <windows.h>
#include <shlobj.h>
#include <chrono>
#include <ctime>

using namespace std;

/// <summary>
/// Handles the launching and stopping of a process
/// </summary>
class ProcessMan {
public:

	ProcessMan();

	void start(string path, string args);
	void stop(bool kill = false);
	void restart();
	static void open(string path, string args);

	static ProcessMan instance;

private:
	thread processThread;

	LPCWSTR filePath;
	LPCWSTR fileArgs;

	void launch();

	bool isRunning = false;
	bool isRestarting = false;
	DWORD processId = 0;

};


