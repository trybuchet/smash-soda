#pragma once

#include "../imgui/imgui.h"
#include "../services/ThemeController.h"
#include "../models/Theme.h"

class TitleTooltipWidget
{
public:
	static bool render(const char* title, const char* description, bool forceShow = false);
};




