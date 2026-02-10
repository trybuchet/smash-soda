#include "PathHelper.h"
#include "helpers/WStringConvert.h"

/**
 * Gets the path to the Smash Soda config.json file.
 */
string PathHelper::GetConfigPath() {
	static string cachedConfigPath = "";
	if (!cachedConfigPath.empty()) return cachedConfigPath;

	string dirPath = "";
	string appDir = "\\Trybuchet\\Smash Soda\\";

	// If running in portable mode
	string currentPath = PathHelper::GetCurrentPath();
	if (MTY_FileExists(string(currentPath + "\\portable.txt").c_str())) {
		dirPath = currentPath + appDir;
	}
	else {

		// Get the appdata path
		string appDataPath = PathHelper::GetAppDataPath();
		if (!appDataPath.empty()) {
			dirPath = appDataPath + appDir;
		}
		else {
			return "";
		}
	}

	bool isDirOk = false;
	if (!MTY_FileExists(dirPath.c_str())) {
		if (MTY_Mkdir(dirPath.c_str())) {
			isDirOk = true;
		}
	}
	else isDirOk = true;
	if (isDirOk) cachedConfigPath = dirPath;

	return dirPath;
}

/**
 * Get the current path Smash Soda is running from.
 */
string PathHelper::GetCurrentPath() {
	char cwd[1024];
	if (_getcwd(cwd, sizeof(cwd)) != NULL) {
		return string(cwd);
	}
	else {
		return "";
	}
}

/**
 * Get the path to AppData directory.
 */
string PathHelper::GetAppDataPath() {
	TCHAR tAppdata[1024];
	if (SUCCEEDED(SHGetFolderPath(nullptr, CSIDL_APPDATA, NULL, 0, tAppdata))) {
		wstring wAppdata(tAppdata);
				string dirPath = WStringToString(wAppdata);

		return dirPath;
	}
}

std::string PathHelper::ConvertTCHARToString(const TCHAR* tcharStr) {
#ifdef UNICODE
	std::wstring wstr(tcharStr);
	return WStringToString(wstr);
#else
	return std::string(tcharStr);
#endif
}

std::wstring PathHelper::ConvertStringToWString(const std::string& str) {
	return std::wstring(str.begin(), str.end());
}

std::vector<std::string> PathHelper::GetFilenames(const std::string& directoryPath, bool includeExtension) {
	std::vector<std::string> filenames;
	WIN32_FIND_DATA findFileData;
#ifdef UNICODE
	std::wstring wDirectoryPath = ConvertStringToWString(directoryPath);
	HANDLE hFind = FindFirstFile((wDirectoryPath + L"\\*").c_str(), &findFileData);
#else
	HANDLE hFind = FindFirstFile((directoryPath + "\\*").c_str(), &findFileData);
#endif

	if (hFind == INVALID_HANDLE_VALUE) {
		std::cerr << "Failed to open directory: " << directoryPath << std::endl;
		return filenames;
	}

	do {
		const std::string fileOrDirName = ConvertTCHARToString(findFileData.cFileName);
		if (!(findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
			std::string filename = fileOrDirName;
			if (!includeExtension) {
				size_t lastDot = fileOrDirName.find_last_of(".");
				filename = (lastDot == std::string::npos) ? fileOrDirName : fileOrDirName.substr(0, lastDot);
			}
			filenames.push_back(filename);
		}
	} while (FindNextFile(hFind, &findFileData) != 0);

	FindClose(hFind);
	return filenames;
}


