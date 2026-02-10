#pragma once
#pragma once

#include "../ImGui/imgui.h"
#include "../Hosting.h"
#include "../globals/AppIcons.h"
#include "../globals/AppFonts.h"
#include "Widget.h"

class HotseatWidget : public Widget {
public:

	HotseatWidget(Hosting& hosting);
	bool render(bool& showWindow);

	void renderOverview();
	void renderSettings();

private:

	// Hotseat
	int _playTime = 0;
	int _resetTime = 0;
	int _minResetTime = 0;
	int _reminderInterval = 0;

	// Dependency injection
	Hosting& _hosting;

};


