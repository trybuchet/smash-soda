#pragma once

#include "../imgui/imgui.h"
#include "../Hosting.h"
#include "../globals/AppIcons.h"
#include "Stopwatch.h"
#include "ToggleIconButtonWidget.h"
#include "TitleTooltipWidget.h"
#include "AnimatedGamepadWidget.h"
#include "LoadingRingWidget.h"
#include "IntRangeWidget.h"
#include "LoadingRingWidget.h"
#include "Widget.h"

class GamepadsWidget : public Widget
{
public:
	GamepadsWidget(Hosting& hosting);
	bool render(bool& showWindow);

private:
	// Dependency injection
	Hosting& _hosting;
	
	// Attributes
	string _logBuffer;
	vector<AGamepad*>& _gamepads;

	void renderPadInputTypeIcon(AGamepad* pad, const size_t& gamepadIndex, bool& refreshGamepads);
};


