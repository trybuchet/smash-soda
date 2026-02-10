#include "../Hosting.h"
extern Hosting g_hosting;
#include "ProcessMan.h"

ProcessMan ProcessMan::instance = ProcessMan();

/// <summary>
/// Constructor
/// </summary>
ProcessMan::ProcessMan() {

}

/// <summary>
/// Launches an application.
/// </summary>
/// <param name="path"></param>
/// <param name="args"></param>
void ProcessMan::start(string path, string args) {

    // If already running, stop the process
	if (isRunning) {
		stop(true);
	}

	// Convert path and args to LPCWSTR
	std::wstring wpath(path.begin(), path.end());
	std::wstring wargs(args.begin(), args.end());
    filePath = wpath.c_str();
    fileArgs = wargs.c_str();

	// Launch the process
	launch();

	// Start the process thread anonymous lambda function
	isRunning = true;
	processThread = thread([this]() {
		while (isRunning) {
			HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, processId);
			if (hProcess == NULL) {
				// Log error or handle invalid process ID
				break;
			}

			DWORD exitCode;
			if (!GetExitCodeProcess(hProcess, &exitCode)) {
				// Log error
				CloseHandle(hProcess);
				break;
			}
			CloseHandle(hProcess);

			if (exitCode != STILL_ACTIVE) {
				launch();
			}
			this_thread::sleep_for(chrono::seconds(5));
		}
	});
	processThread.detach();

}

/// <summary>
/// Stops the application.
/// </summary>
void ProcessMan::stop(bool kill) {
	if (!isRunning) {
		return;
	}

	isRunning = false;
	// Terminate the process
	if (kill) {
		TerminateProcess(OpenProcess(PROCESS_TERMINATE, FALSE, processId), 0);
		// Stop the process thread
		if (processThread.joinable()) {
			processThread.join();
		}
	}
}

/// <summary>
/// Launches the process.
/// </summary>
void ProcessMan::launch() {
	// Create the process
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	// Start the child process. 
	if (!CreateProcess(filePath,	// No module name (use command line)
		(LPWSTR)fileArgs,			// Command line
		NULL,						// Process handle not inheritable
		NULL,						// Thread handle not inheritable
		FALSE,						// Set handle inheritance to FALSE
		0,							// No creation flags
		NULL,						// Use parent's environment block
		NULL,						// Use parent's starting directory 
		&si,						// Pointer to STARTUPINFO structure
		&pi)						// Pointer to PROCESS_INFORMATION structure
		) {
		
		g_hosting.logMessage("Kiosk Mode failed to start! Please check the path and arguments.");
		isRunning = false;
		Config::cfg.kioskMode.enabled = false;

		return;
	}

	// Save the process ID
	processId = pi.dwProcessId;

}

/// <summary>
/// Restarts the application.
/// </summary>
void ProcessMan::restart() {
	TerminateProcess(OpenProcess(PROCESS_TERMINATE, FALSE, processId), 0);
	// Stop the process thread
	if (processThread.joinable()) {
		processThread.join();
	}
	launch();
}

void ProcessMan::open(string path, string args) {

	// Create the process
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	// Convert path and args to LPCWSTR
	std::wstring wpath(path.begin(), path.end());
	std::wstring wargs(args.begin(), args.end());
    LPCWSTR lpath = wpath.c_str();
    LPCWSTR largs = wargs.c_str();
	DWORD pId = 0;

	// Start the child process. 
	if (!CreateProcess(lpath,		// No module name (use command line)
		(LPWSTR)largs,				// Command line
		NULL,						// Process handle not inheritable
		NULL,						// Thread handle not inheritable
		FALSE,						// Set handle inheritance to FALSE
		0,							// No creation flags
		NULL,						// Use parent's environment block
		NULL,						// Use parent's starting directory 
		&si,						// Pointer to STARTUPINFO structure
		&pi)						// Pointer to PROCESS_INFORMATION structure
		) {
		return;
	}

	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pId);
	if (hProcess == NULL) {
		// Log error or handle invalid process ID
		return;
	}

	DWORD exitCode;
	if (!GetExitCodeProcess(hProcess, &exitCode)) {
		// Log error
		CloseHandle(hProcess);
	}
	CloseHandle(hProcess);

}


