#pragma once

#include <iostream>
#include <string>
#include <filesystem>
#include "../ImGui/imgui.h"
#include "../Hosting.h"
#include "../globals/AppIcons.h"
#include "IconButton.h"
#include "TitleTooltipWidget.h"
#include "ConfirmPopupWidget.h"
#include "PopupWidgetEdit.h"
#include "../Core/Cache.h"
#include "Widget.h"

class LibraryWidget : public Widget
{
public:

	LibraryWidget(Hosting& hosting);
	bool render(bool& showWindow);
	bool renderGameList();
	bool renderForm(int index);

private:

	std::string openFileDialog();
	
	// Dependency injection
	Hosting& _hosting;

	bool showEditForm = false;

	int activeIndex = -1;
	int popupRemoveIndex = -1;
	int popupEditIndex = -1;

	char _filterText[256] = "";

	char _editName[256] = "";
	char _editPath[256] = "";
	char _editID[256] = "1wdoHfhhZH5lPuZCwGBete0HIAj";
	char _editParam[256] = "";
	char _editThumb[256] = "";
	bool _editKiosk = false;
	bool _editHotseat = false;
	int _editSeats = 1;

};


