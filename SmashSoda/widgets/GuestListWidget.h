#pragma once

#include "../imgui/imgui.h"
#include "../Hosting.h"
#include "../globals/AppIcons.h"
#include "IconButton.h"
#include "TitleTooltipWidget.h"
#include "ConfirmPopupWidget.h"
#include "PopupWidgetEdit.h"
#include "Widget.h"
#include "../services/ThemeController.h"
#include "../models/Theme.h"

class GuestListWidget : public Widget
{
public:
	GuestListWidget(Hosting& hosting);
	bool render(bool& showWindow);

private:
	void renderOnlineGuests();
	void renderBannedGuests();
	void renderHistoryGuests();
	void renderModdedGuests();
	void renderVIPGuests();

	void guestItem(Guest& guest);

	// Dependency injection
	Hosting& _hosting;
	
	// Attributes
	char _filterText[256] = "";
	bool _filterNoReason = false;
	string _logBuffer;
	vector<Guest>& _guests;
	vector<GuestData>& _guestHistory;
};


