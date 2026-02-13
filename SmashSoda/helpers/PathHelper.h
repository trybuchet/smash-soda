#pragma once

#include <string>
#include <direct.h>
#include <iostream>
#include <windows.h>
#include <shlobj.h>
#include "matoya.h"
#include <tchar.h>
#include <nlohmann/json.hpp>

using namespace std;

class PathHelper {
public:
    static string GetConfigPath();
    static string GetCurrentPath();
    static string GetAppDataPath();
    static std::vector<std::string> GetFilenames(const std::string& directoryPath, bool includeExtension);
private:
    static std::string ConvertTCHARToString(const TCHAR* tcharStr);
    static std::wstring ConvertStringToWString(const std::string& str);
};


