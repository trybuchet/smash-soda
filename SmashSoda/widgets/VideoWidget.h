#pragma once

#include "../imgui/imgui.h"
#include "../Hosting.h"
#include "Debouncer.h"
#include "../globals/AppIcons.h"
#include "../globals/AppFonts.h"
#include "TitleTooltipWidget.h"
#include "TooltipWidget.h"
#include "IntRangeWidget.h"
#include "Widget.h"

class VideoWidget : public Widget
{
public:
	VideoWidget(Hosting& hosting);
	bool render(bool& showWindow);

private:
	// Dependency injection
	Hosting& _hosting;
	DX11& _dx11;
	uint32_t DEBOUNCER_TIME_MS = 1000;

	int _fps = 60;
	int _bandwidth = 20;
	
};


