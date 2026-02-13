#pragma once

#define _WINSOCKAPI_

#include "../imgui/imgui.h"
#include "../globals/AppIcons.h"
#include "../Hosting.h"
#include "IconButton.h"
#include "ToggleIconButtonWidget.h"
#include "TitleTooltipWidget.h"
#include "../services/ThemeController.h"
#include "../models/Theme.h"
#include "../helpers/PathHelper.h"

class NavBar
{
public:
	static void render(
        Hosting& hosting,
        bool& showLogin,
        bool& showHostSettings,
        bool& showGamepads,
        bool& showMasterOfPuppets,
        bool& showChat,
        bool& showGuests,
        bool& showLog,
        bool& showAudio,
        bool& showVideo,
        bool& showInfo,
        bool& showSettings,
        bool& showButtonLock,
        bool& showLibrary,
        bool& showOverlay,
        bool& showHotseat,
        bool& showKeyMap,
        bool& showDevTools,
        bool& showStream
    );//-- CodeSomnia Moidified --
      // CodeSomnia : showKeyMap Added

private:
    static void renderNavtooltip(const char* title, bool& show);
};

