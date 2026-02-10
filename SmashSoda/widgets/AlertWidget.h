#pragma once

#include "../imgui/imgui.h"
#include "../globals/AppIcons.h"
#include "Widget.h"

class AlertWidget : public Widget
{
public:
	static bool render(const char* title, const char* message);
};




