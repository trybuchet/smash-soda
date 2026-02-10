#pragma once

#include "../imgui/imgui.h"
#include "../Hosting.h"
#include "../globals/AppIcons.h"
#include "Widget.h"

class LogWidget : public Widget
{
public:
	LogWidget(Hosting& hosting);
	bool render(bool& showWindow);

private:
	string _logBuffer;
	vector<string>& _commandLog;
	size_t _messageCount;
};




