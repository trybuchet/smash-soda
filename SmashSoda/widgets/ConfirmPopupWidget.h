#pragma once

#include <string>
#include "../imgui/imgui.h"
#include "../globals/AppIcons.h"
#include "IconButton.h"
#include "Widget.h"

class ConfirmPopupWidget : public Widget
{
public:
	static bool render(const char* title, bool& showPopup, std::string textString = "");
};
