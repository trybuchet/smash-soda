#pragma once

#include "../imgui/imgui.h"
#include "../Hosting.h"
#include "Debouncer.h"
#include "../globals/AppIcons.h"
#include "../globals/AppFonts.h"
#include "TitleTooltipWidget.h"
#include "TooltipWidget.h"
#include "IntRangeWidget.h"
#include "../ButtonLock.h"
#include "Widget.h"
#include "../services/ThemeController.h"
#include "../models/Theme.h"

class ButtonLockWidget : public Widget
{
public:
	ButtonLockWidget(Hosting& hosting);
	LockedGamepadState lockedGamepad;
	ImVec2 smallButton{ 25, 25 };
	ImVec2 wideButton{ 60, 25 };
	ImVec2 ultrawideButton{ 125, 25 };
	ImVec2 bigButton{ 60, 55 };
	bool isFocused = false;
	void setColor(float hue = 0.35f);
	void makeButton(string buttonText, ImVec2 size, uint16_t bitwise);
	void makeButton(string buttonText, ImVec2 size, bool& boolToChange);
	void makeButton(string buttonText, ImVec2 size, bool& boolToChange1, bool& boolToChange2);
	bool render(bool& showWindow);
	void updateButtonLock(LockedGamepadState lockedGamepad);

private:
	// Dependency injection
	Hosting& _hosting;
};



