#pragma once

#include <string>
#include <sstream>
#include "imgui/imgui.h"
#include "globals/AppFonts.h"
#include <nlohmann/json.hpp>

using namespace std;
using json = nlohmann::json;

class Theme {
public:
	explicit Theme(const string& name = "Default", const json& theme = json{});

	// Fonts
	ImFont* fontLight;
	ImFont* fontRegular;
	ImFont* fontMedium;
	ImFont* fontSemiBold;
	ImFont* fontBold;

    // Colours
    string name;

    ImVec4 title;
	ImVec4 label;
	ImVec4 input;
	ImVec4 positive;
	ImVec4 negative;
	ImVec4 warning;

	ImVec4 white;
	ImVec4 black;
	ImVec4 invisible;

	ImVec4 primary;
	ImVec4 secondary;

	ImVec4 backgroundIcon;

	ImVec4 disabled;

	ImVec4 background;

	ImVec4 panelBackground;
	ImVec4 panelBorder;
	ImVec4 panelBorderActive;
	ImVec4 panelText;
	ImVec4 panelTitleBar;
	ImVec4 panelTitleBarText;
	ImVec4 panelTitleBarActive;
	ImVec4 panelTitleBarActiveText;
	ImVec4 panelFooter;
	ImVec4 panelFooterText;

	ImVec4 text;
	ImVec4 textMuted;
	ImVec4 textHighlighted;

	ImVec4 listItemBackground;

	ImVec4 tabsBackground;
	ImVec4 tabsBorder;
	ImVec4 tab;
	ImVec4 tabText;
	ImVec4 tabHovered;
	ImVec4 tabActive;
	ImVec4 tabActiveText;

	ImVec4 navbarBackground;
	ImVec4 navbarBorder;
	ImVec4 navbarIcon;
	ImVec4 navbarIconActive;

	ImVec4 formLabel;
	ImVec4 formInputBackground;
	ImVec4 formInputBorder;
	ImVec4 formInputBorderActive;
	ImVec4 formInputText;
	ImVec4 formInputTextActive;
	ImVec4 formHelpText;

	ImVec4 buttonPrimary;
	ImVec4 buttonPrimaryBorder;
	ImVec4 buttonPrimaryText;
	ImVec4 buttonPrimaryHovered;
	ImVec4 buttonPrimaryHoveredText;
	ImVec4 buttonPrimaryActive;
	ImVec4 buttonPrimaryActiveText;
	ImVec4 buttonPrimaryDisabled;

	ImVec4 buttonSecondary;
	ImVec4 buttonSecondaryBorder;
	ImVec4 buttonSecondaryText;
	ImVec4 buttonSecondaryHovered;
	ImVec4 buttonSecondaryHoveredText;
	ImVec4 buttonSecondaryActive;
	ImVec4 buttonSecondaryActiveText;
	ImVec4 buttonSecondaryDisabled;

	ImVec4 buttonEnable;
	ImVec4 buttonEnableBorder;
	ImVec4 buttonEnableText;
	ImVec4 buttonEnableHovered;
	ImVec4 buttonEnableHoveredText;
	ImVec4 buttonEnableActive;
	ImVec4 buttonEnableActiveText;
	ImVec4 buttonEnableDisabled;

	ImVec4 buttonDisable;
	ImVec4 buttonDisableBorder;
	ImVec4 buttonDisableText;
	ImVec4 buttonDisableHovered;
	ImVec4 buttonDisableHoveredText;
	ImVec4 buttonDisableActive;
	ImVec4 buttonDisableActiveText;
	ImVec4 buttonDisableDisabled;

	ImVec4 scrollbarBackground;
	ImVec4 scrollbarHandle;
	ImVec4 scrollbarHandleHovered;
	ImVec4 scrollbarHandleActive;

	ImVec4 sliderGrab;
	ImVec4 sliderGrabActive;

    // Apply full ImGui style
    virtual void apply();
	bool loadFromJSON(const json& theme);

	// Helpers
	ImVec4 hexToImVec4(const std::string& hex);
	ImVec4 rgbaToImVec4(const std::string& rgba);
	ImVec4 parseColorString(const std::string& colorString, ImVec4 defaultColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
};
