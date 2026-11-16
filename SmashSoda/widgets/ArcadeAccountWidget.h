#pragma once

#include <string>
#include <thread>
#include <atomic>
#include <mutex>
#include "../imgui/imgui.h"
#include "../globals/AppIcons.h"
#include "../Core/Config.h"
#include "../Core/Cache.h"
#include "../services/Arcade.h"
#include "../services/ThemeController.h"
#include "../models/Theme.h"
#include "LoadingRingWidget.h"
#include "Widget.h"

/// <summary>
/// Soda Arcade account requirements Smash Soda must collect before a room can be listed:
/// the one-time account setup (date of birth + Terms) and the hosting rules.
/// Mirrors what the Soda Arcade app asks on first login and before hosting.
/// </summary>
class ArcadeAccountWidget : public Widget
{
public:
	ArcadeAccountWidget();

	/// Account setup window (date of birth and/or updated Terms). Blocks other UI while shown.
	void renderOnboarding();

	/// Hosting rules window with the required agreement checkbox.
	void renderHostingRules();

private:
	void beginWindow(const char* title, float width, float height);
	void endWindow();
	void openWebPage(const std::string& path);
	bool validDateOfBirth(std::string& isoDate);
	void setError(const std::string& message);
	std::string getError();
	static void relistRoomIfPublic();

	std::atomic<bool> _busy{ false };
	std::mutex _errorMutex;
	std::string _error;

	// Date of birth (string values for elSelect)
	std::string _day = "";
	std::string _month = "";
	std::string _year = "";
	bool _agreeTerms = false;

	bool _agreeHostingRules = false;
};
