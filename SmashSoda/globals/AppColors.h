#pragma once

#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_win32.h"
#include "../imgui/imgui_impl_dx11.h"

#define COLOR_PRIMARY_U32 ImGui::GetColorU32(IM_COL32(0, 120, 204, 255))
#define COLOR_POSITIVE_U32 ImGui::GetColorU32(IM_COL32(0, 171, 105, 255))
#define COLOR_NEGATIVE_U32 ImGui::GetColorU32(IM_COL32(191, 41, 71, 255))
#define COLOR_WIDGET_BG_U32 ImGui::GetColorU32(IM_COL32(20, 20, 20, 255))

class AppColors
{
public:
	static void init();

	static ImVec4 alpha(ImVec4& color, float w);
	static ImU32 toU32(ImVec4& color);

	static void pushColor(ImVec4& color);
	static void pop();

	static void pushPrimary();
	static void pushSecondary();

	static void pushTitle();
	static void pushLabel();
	static void pushInput();
	static void pushPositive();
	static void pushNegative();
	static void pushWarning();
	static void pushButton();
	static void pushButtonSolid();
	static void pushButtonSecondary();
	static void pushButtonDanger();
	static void popButton();

	static ImVec4 title;
	static ImVec4 label;
	static ImVec4 input;
	static ImVec4 positive;
	static ImVec4 negative;
	static ImVec4 warning;

	static ImVec4 white;
	static ImVec4 black;
	static ImVec4 invisible;

	static ImVec4 primary;
	static ImVec4 secondary;

	static ImVec4 backgroundIcon;

	static ImVec4 disabled;

	static ImVec4 background;

	static ImVec4 panelBackground;
	static ImVec4 panelBorder;
	static ImVec4 panelBorderActive;
	static ImVec4 panelText;
	static ImVec4 panelTitleBar;
	static ImVec4 panelTitleBarText;
	static ImVec4 panelTitleBarActive;
	static ImVec4 panelTitleBarActiveText;
	static ImVec4 panelFooter;
	static ImVec4 panelFooterText;

	static ImVec4 tabsBackground;
	static ImVec4 tabsBorder;
	static ImVec4 tab;
	static ImVec4 tabText;
	static ImVec4 tabHovered;
	static ImVec4 tabActive;
	static ImVec4 tabActiveText;

	static ImVec4 navbarBackground;
	static ImVec4 navbarBorder;
	static ImVec4 navbarIcon;
	static ImVec4 navbarIconActive;

	static ImVec4 formLabel;
	static ImVec4 formInputBackground;
	static ImVec4 formInputBorder;
	static ImVec4 formInputBorderActive;
	static ImVec4 formInputText;
	static ImVec4 formInputTextActive;
	static ImVec4 formHelpText;

	static ImVec4 buttonPrimary;
	static ImVec4 buttonPrimaryBorder;
	static ImVec4 buttonPrimaryText;
	static ImVec4 buttonPrimaryHovered;
	static ImVec4 buttonPrimaryHoveredText;
	static ImVec4 buttonPrimaryActive;
	static ImVec4 buttonPrimaryActiveText;
	static ImVec4 buttonPrimaryDisabled;

	static ImVec4 buttonSecondary;
	static ImVec4 buttonSecondaryBorder;
	static ImVec4 buttonSecondaryText;
	static ImVec4 buttonSecondaryHovered;
	static ImVec4 buttonSecondaryHoveredText;
	static ImVec4 buttonSecondaryActive;
	static ImVec4 buttonSecondaryActiveText;
	static ImVec4 buttonSecondaryDisabled;

};