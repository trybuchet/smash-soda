#pragma once

#include "../ImGui/imgui.h"
#include "../Hosting.h"
#include "../globals/AppIcons.h"
#include "IconButton.h"
#include "TitleTooltipWidget.h"
#include "ConfirmPopupWidget.h"
#include "PopupWidgetEdit.h"

class OverlayWidget
{
public:

	OverlayWidget(Hosting& hosting);
	bool render(bool& showWindow);

private:

	float _opacity = 0.9;
	
	bool _chat_enabled = true;
	bool _show_history = true;
	string _chat_position = "top left";

	bool _pads_enabled = false;
	string _pads_position = "bottom center";

	bool _guests_enabled = false;
	bool _show_latency = true;
	string _guests_position = "top right";

	string _position_options[6] = {"top Left", "top center", "top right", "bottom left", "bottom center", "bottom right"};

	// Dependency injection
	Hosting& _hosting;
	DX11& _dx11;

};


