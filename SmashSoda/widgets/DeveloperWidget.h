#pragma once
#pragma once

#include "../ImGui/imgui.h"
#include "../Hosting.h"
#include "../globals/AppIcons.h"
#include "IconButton.h"
#include "TitleTooltipWidget.h"
#include "ConfirmPopupWidget.h"
#include "PopupWidgetEdit.h"
#include "../ImGui/imform.h"

class DeveloperWidget {
public:

	DeveloperWidget(Hosting& hosting);
	bool render(bool& showWindow);

private:

	// Dependency injection
	Hosting& _hosting;

	char _stagingDomain[256];
	char _devDomain[256];

	bool _useDevDomain = false;
	bool _useStagingDomain = false;

};


