#pragma once

#define _WINSOCKAPI_

#include "../imgui/imgui.h"
#include "../globals/AppIcons.h"
#include "../Hosting.h"
#include "../Helpers/Clock.h"
#include "../Helpers/Stopwatch.h"
#include "../Helpers/Debouncer.h"
#include "TitleTooltipWidget.h"
#include "ProgressCircularWidget.h"
#include "Widget.h"
#include "../models/SessionCache.h"
#include "../services/ThemeController.h"
#include "../models/Theme.h"

class HostInfoWidget : public Widget
{
public:
	HostInfoWidget(Hosting& hosting);
	void render();
	void renderArcade();

private:
	Hosting& _hosting;
	uint32_t _loginTime;
};

