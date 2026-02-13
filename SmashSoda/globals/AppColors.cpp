#include "AppColors.h"

ImVec4 AppColors::title;
ImVec4 AppColors::label;
ImVec4 AppColors::input;
ImVec4 AppColors::positive;
ImVec4 AppColors::negative;
ImVec4 AppColors::warning;

ImVec4 AppColors::white;
ImVec4 AppColors::black;
ImVec4 AppColors::invisible;

ImVec4 AppColors::primary;
ImVec4 AppColors::secondary;

ImVec4 AppColors::backgroundIcon;

ImVec4 AppColors::disabled;

ImVec4 AppColors::background;

ImVec4 AppColors::panelBackground;
ImVec4 AppColors::panelBorder;
ImVec4 AppColors::panelBorderActive;
ImVec4 AppColors::panelText;
ImVec4 AppColors::panelTitleBar;
ImVec4 AppColors::panelTitleBarText;
ImVec4 AppColors::panelTitleBarActive;
ImVec4 AppColors::panelTitleBarActiveText;
ImVec4 AppColors::panelFooter;
ImVec4 AppColors::panelFooterText;

ImVec4 AppColors::tabsBackground;
ImVec4 AppColors::tabsBorder;
ImVec4 AppColors::tab;
ImVec4 AppColors::tabText;
ImVec4 AppColors::tabHovered;
ImVec4 AppColors::tabActive;
ImVec4 AppColors::tabActiveText;

ImVec4 AppColors::navbarBackground;
ImVec4 AppColors::navbarBorder;
ImVec4 AppColors::navbarIcon;
ImVec4 AppColors::navbarIconActive;

ImVec4 AppColors::formLabel;
ImVec4 AppColors::formInputBackground;
ImVec4 AppColors::formInputBorder;
ImVec4 AppColors::formInputBorderActive;
ImVec4 AppColors::formInputText;
ImVec4 AppColors::formInputTextActive;
ImVec4 AppColors::formHelpText;

ImVec4 AppColors::buttonPrimary;
ImVec4 AppColors::buttonPrimaryBorder;
ImVec4 AppColors::buttonPrimaryText;
ImVec4 AppColors::buttonPrimaryHovered;
ImVec4 AppColors::buttonPrimaryHoveredText;
ImVec4 AppColors::buttonPrimaryActive;
ImVec4 AppColors::buttonPrimaryActiveText;
ImVec4 AppColors::buttonPrimaryDisabled;

ImVec4 AppColors::buttonSecondary;
ImVec4 AppColors::buttonSecondaryBorder;
ImVec4 AppColors::buttonSecondaryText;
ImVec4 AppColors::buttonSecondaryHovered;
ImVec4 AppColors::buttonSecondaryHoveredText;
ImVec4 AppColors::buttonSecondaryActive;
ImVec4 AppColors::buttonSecondaryActiveText;
ImVec4 AppColors::buttonSecondaryDisabled;

void AppColors::init() {

	panelBackground = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
	panelBorder = ImVec4(1.0f, 1.0f, 1.0f, 0.20f);
	panelBorderActive = ImVec4(0.69f, 0.26f, 0.16f, 1.00f);
	panelText = ImVec4(0.95f, 0.93f, 0.83f, 1.00f);
	panelTitleBar = ImVec4(0.95f, 0.93f, 0.83f, 1.00f);
	panelTitleBarText = ImVec4(0.19f, 0.18f, 0.17f, 1.00f);
	panelTitleBarActive = ImVec4(0.69f, 0.26f, 0.16f, 1.00f);
	panelTitleBarActiveText = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	panelFooter = ImVec4(0.16f, 0.15f, 0.14f, 1.00f);
	panelFooterText = ImVec4(0.70f, 0.70f, 0.70f, 1.00f);

	tabsBackground = ImVec4(0.16f, 0.15f, 0.14f, 1.00f);
	tabsBorder = ImVec4(0.69f, 0.26f, 0.16f, 1.00f);
	tab = ImVec4(0.95f, 0.93f, 0.83f, 1.00f);
	tabText = ImVec4(0.19f, 0.18f, 0.17f, 1.00f);
	tabHovered = ImVec4(0.71f, 0.28f, 0.18f, 1.00f);
	tabActive = ImVec4(0.69f, 0.26f, 0.16f, 1.00f);
	tabActiveText = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);

	navbarBackground = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
	navbarBorder = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
	navbarIcon = ImVec4(1.00f, 1.00f, 1.00f, 0.15f);
	navbarIconActive = ImVec4(0.78f, 0.40f, 0.33f, 1.00f);

	formLabel = ImVec4(0.95f, 0.93f, 0.83f, 1.00f);
	formInputBackground = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
	formInputBorder = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
	formInputBorderActive = ImVec4(0.00f, 0.47f, 0.80f, 1.00f);
	formInputText = ImVec4(0.70f, 0.70f, 0.70f, 1.00f);
	formInputTextActive = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	formHelpText = ImVec4(0.91f, 0.80f, 0.25f, 1.00f);

	buttonPrimary = ImVec4(0.69f, 0.26f, 0.16f, 1.00f);
	buttonPrimaryBorder = ImVec4(0.69f, 0.26f, 0.16f, 1.00f);
	buttonPrimaryText = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	buttonPrimaryHovered = ImVec4(0.71f, 0.28f, 0.18f, 1.00f);
	buttonPrimaryHoveredText = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	buttonPrimaryActive = ImVec4(0.72f, 0.28f, 0.18f, 1.00f);
	buttonPrimaryActiveText = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	buttonPrimaryDisabled = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);

	buttonSecondary = ImVec4(0.95f, 0.93f, 0.83f, 1.00f);
	buttonSecondaryBorder = ImVec4(0.00f, 0.00f, 0.00f, 0.20f);
	buttonSecondaryText = ImVec4(0.19f, 0.18f, 0.17f, 1.00f);
	buttonSecondaryHovered = ImVec4(0.98f, 0.95f, 0.85f, 1.00f);
	buttonSecondaryHoveredText = ImVec4(0.19f, 0.18f, 0.17f, 1.00f);
	buttonSecondaryActive = ImVec4(0.00f, 0.47f, 0.80f, 1.00f);
	buttonSecondaryActiveText = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	buttonSecondaryDisabled = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);

	positive = ImVec4(0.55f, 0.75f, 0.55f, 1.00f); // Green / Positive
	negative = ImVec4(0.80f, 0.45f, 0.45f, 1.00f); // Red / Negative

	background = ImVec4(0.1176f, 0.1098f, 0.1059f, 1.00f); // Dark background
	primary = buttonSecondary;
	secondary = buttonSecondary;
	disabled = ImVec4(0.06f, 0.06f, 0.06f, 1.00f); // Muted / Disabled
	invisible = ImVec4(1.00f, 1.00f, 1.00f, 0.00f);

	ImGuiStyle* style = &ImGui::GetStyle();

	// Layout
	style->WindowBorderSize = 1;
	style->PopupBorderSize = 1;
	style->WindowPadding = ImVec2(20, 20);
	style->ItemSpacing = ImVec2(5, 5);
	style->ScrollbarSize = 20;
	style->GrabMinSize = 20;

	style->WindowRounding = 8;
	style->FrameRounding = 8;
	style->PopupRounding = 8;
	style->ScrollbarRounding = 8;
	style->GrabRounding = 2;
	style->WindowTitleAlign = ImVec2(0.5f, 0.5f);

	style->DisplaySafeAreaPadding = ImVec2(20, 20);

	ImVec4* colors = ImGui::GetStyle().Colors;
	colors[ImGuiCol_Text] = panelText;
	colors[ImGuiCol_TitleBg] = panelTitleBar;
	colors[ImGuiCol_TitleBgCollapsed] = panelTitleBar;
	colors[ImGuiCol_TitleBgActive] = panelTitleBarActive;
	colors[ImGuiCol_WindowBg] = panelBackground;
	colors[ImGuiCol_Border] = panelBorder;
	colors[ImGuiCol_FrameBg] = formInputBackground;
	colors[ImGuiCol_FrameBgHovered] = formInputBackground;
	colors[ImGuiCol_FrameBgActive] = formInputBackground;
	colors[ImGuiCol_Button] = buttonSecondary;
	colors[ImGuiCol_ButtonHovered] = buttonSecondaryHovered;
	colors[ImGuiCol_ButtonActive] = buttonSecondaryActive;
	colors[ImGuiCol_ScrollbarBg] = background;
	colors[ImGuiCol_ScrollbarGrab] = panelTitleBar;
	colors[ImGuiCol_ResizeGrip] = buttonSecondary;
	colors[ImGuiCol_ResizeGripHovered] = buttonSecondaryHovered;
	colors[ImGuiCol_ResizeGripActive] = buttonSecondaryHovered;
	colors[ImGuiCol_CheckMark] = panelText;

	colors[ImGuiCol_Tab] = tab;
	colors[ImGuiCol_TabHovered] = tabHovered;
	colors[ImGuiCol_TabActive] = tabActive;
	colors[ImGuiCol_TabUnfocused] = tab;
	colors[ImGuiCol_TabUnfocusedActive] = tabActive;

	colors[ImGuiCol_DragDropTarget] = ImVec4(0.3f, 0.3f, 0.3f, 0.4f);

}

ImVec4 AppColors::alpha(ImVec4& color, float alpha) {
	return ImVec4(color.x, color.y, color.z, alpha);
}

ImU32 AppColors::toU32(ImVec4& color) {
	return ImGui::GetColorU32(IM_COL32(255 * color.x, 255 * color.y, 255 * color.z, 255 * color.w));
}

void AppColors::pushColor(ImVec4& color) {
	ImGui::PushStyleColor(ImGuiCol_Text, color);
}

void AppColors::pop() {
	ImGui::PopStyleColor();
}

void AppColors::pushPrimary() {
	pushColor(buttonSecondary);
}

void AppColors::pushSecondary() {
	pushColor(buttonSecondary);
}

void AppColors::pushTitle() {
	pushColor(panelText);
}

void AppColors::pushLabel() {
	pushColor(formLabel);
}

void AppColors::pushInput() {
	pushColor(formInputText);
}

void AppColors::pushPositive() {
	pushColor(positive);
}

void AppColors::pushNegative() {
	pushColor(negative);
}

void AppColors::pushButton() {
	pushColor(white);
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.00f, 1.00f, 1.00f, 0.00f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.00f, 1.00f, 1.00f, 0.08f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.00f, 1.00f, 1.00f, 0.08f));
}

void AppColors::pushButtonSolid() {
	pushColor(black);
	ImGui::PushStyleColor(ImGuiCol_Button, positive);
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, positive);
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, primary);
}

void AppColors::pushButtonSecondary() {
	pushColor(black);
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 1.0f, 1.0f, 1.00f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(.90f, .90f, .90f, 1.00f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(.90f, .90f, .90f, 1.00f));
}

void AppColors::pushButtonDanger() {
	pushColor(black);
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.65f, 0.32f, 0.27f, 1.00f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.70f, 0.35f, 0.32f, 1.00f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.70f, 0.35f, 0.32f, 1.00f));
}

void AppColors::popButton() {
	ImGui::PopStyleColor(4);
}

void AppColors::pushWarning() {
	pushColor(warning);
}
