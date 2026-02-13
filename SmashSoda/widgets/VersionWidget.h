#pragma once

#include <string>
#include <thread>
#include "matoya.h"
#include "../imgui/imgui.h"
#include "../ImGui/imgui_markdown.h"
#include "../globals/AppIcons.h"
#include "LoadingRingWidget.h"
#include "AlertWidget.h"
#include "../ImGui/imform.h"
#include "../Core/Config.h"
#include "../Core/Cache.h"
#include "../services/Arcade.h"
#include "Widget.h"
#include "../services/ThemeController.h"
#include "../models/Theme.h"

#include <nlohmann/json.hpp>
using namespace std;
using json = nlohmann::json;

class VersionWidget : public Widget
{
public:
	bool showUpdate = false;
	bool showDownload = false;
	bool _isLoginLocked = false;
	bool _showError = false;
	std::string version;
	std::string latestVersion;
	std::string changeLog;
	std::string error;
	thread _loginThread;

	char _email[256];
	char _password[256];
	char _2fa[256];

	ImGui::MarkdownConfig mdConfig;
	
	// Constructor
	VersionWidget();

	bool render();
	bool renderLoginWindow();
	bool renderUpdateWindow();
	bool renderDownloadWindow();
};




