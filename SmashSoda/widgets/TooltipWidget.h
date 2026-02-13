#pragma once

#include "../imgui/imgui.h"
#include "../services/ThemeController.h"
#include "../models/Theme.h"

class TooltipWidget
{
public:
	static bool render(const char* text, bool forceShow = false);
};




