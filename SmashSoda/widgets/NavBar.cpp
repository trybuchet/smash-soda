#include "NavBar.h"
#include "../services/OverlayService.h"

void NavBar::render(
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
)//-- CodeSomnia Moidified -- showKeyMap Added
{
	static ImVec2 iconSize = ImVec2(24, 24);
	static ImVec2 windowSize = ImVec2(40, 15*32+8);
	static ImVec2 zero = ImVec2(0, 0);
	static ImVec2 padding = ImVec2(8, 8);
	
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(4, 4));
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0,0));
	//ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0);
	//ImGui::PushStyleVar(ImGuiStyleVar_PopupRounding, 0);

	Theme* theme = ThemeController::getInstance().getActiveTheme();
	ImGui::PushStyleColor(ImGuiCol_WindowBg, theme->panelBackground);
	ImGui::PushStyleColor(ImGuiCol_Border, theme->panelBorderActive);

	ImGui::Begin(
		"##NavBar", (bool*)0,
		ImGuiWindowFlags_NoDecoration |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoNav |
		ImGuiWindowFlags_NoBringToFrontOnFocus
	);
	ImGui::SetWindowSize(windowSize);
	ImGui::SetWindowPos(ImVec2(8, 8));
	
	if (ToggleIconButtonWidget::render(AppIcons::play, AppIcons::play, showHostSettings, iconSize)) {
		showHostSettings = !showHostSettings;
		Config::cfg.widgets.host = showHostSettings;
	}
	renderNavtooltip("Host settings", showHostSettings);

	if (ToggleIconButtonWidget::render(AppIcons::chat, AppIcons::chat, showChat, iconSize)) {
		showChat = !showChat;
		Config::cfg.widgets.chat = showChat;
	}
	renderNavtooltip("Chat", showChat);

	if (ToggleIconButtonWidget::render(AppIcons::users, AppIcons::users, showGuests, iconSize)) {
		showGuests = !showGuests;
		Config::cfg.widgets.guests = showGuests;
	}
	renderNavtooltip("Guests", showGuests);

	if (ToggleIconButtonWidget::render(AppIcons::padOn, AppIcons::padOn, showGamepads, iconSize)) {
		showGamepads = !showGamepads;
		Config::cfg.widgets.gamepads = showGamepads;
	}
	renderNavtooltip("Gamepads", showGamepads);

	if (ToggleIconButtonWidget::render(AppIcons::buttonLock, AppIcons::buttonLock, showButtonLock, iconSize)) {
		showButtonLock = !showButtonLock;
		Config::cfg.widgets.buttonLock = showButtonLock;
	}
	renderNavtooltip("Button Lock", showButtonLock);

	if (ToggleIconButtonWidget::render(AppIcons::puppet, AppIcons::puppet, showMasterOfPuppets, iconSize))
	{
		showMasterOfPuppets = !showMasterOfPuppets;
		Config::cfg.widgets.masterOfPuppets = showMasterOfPuppets;
	}
	renderNavtooltip("Master of Puppets", showMasterOfPuppets);

	//-- CodeSomnia Add Start --

	if (ToggleIconButtonWidget::render(AppIcons::keymap, AppIcons::keymap, showKeyMap, iconSize)) {
		showKeyMap = !showKeyMap;
		Config::cfg.widgets.keyMapper = showKeyMap;
	}
	renderNavtooltip("KeyboardMap", showKeyMap);

	//-- CodeSomnia Add End --
	
	/*if (ToggleIconButtonWidget::render(AppIcons::tournament, AppIcons::tournament, showTournament, iconSize))	showTournament = !showTournament;
	renderNavtooltip("Tournament", showTournament);*/

	if (ToggleIconButtonWidget::render(AppIcons::video, AppIcons::video, showVideo, iconSize)) {
		showVideo = !showVideo;
		Config::cfg.widgets.video = showVideo;
	}
	renderNavtooltip("Video", showVideo);

	if (ToggleIconButtonWidget::render(AppIcons::speakersOn, AppIcons::speakersOn, showAudio, iconSize)) {
		showAudio = !showAudio;
		Config::cfg.widgets.audio = showAudio;
	}
	renderNavtooltip("Audio", showAudio);

	if (ToggleIconButtonWidget::render(AppIcons::hotseat, AppIcons::hotseat, showHotseat, iconSize)) {
		showHotseat = !showHotseat;
		Config::cfg.widgets.hotseat = showHotseat;
	}
	renderNavtooltip("Hotseat", showHotseat);

	if (ToggleIconButtonWidget::render(AppIcons::library, AppIcons::library, showLibrary, iconSize)) {
		showLibrary = !showLibrary;
		Config::cfg.widgets.library = showLibrary;
	}
	renderNavtooltip("Library", showLibrary);

	if (ToggleIconButtonWidget::render(AppIcons::log, AppIcons::log, showLog, iconSize)) {
		showLog = !showLog;
		Config::cfg.widgets.log = showLog;
	}
	renderNavtooltip("Log", showLog);

	if (ToggleIconButtonWidget::render(AppIcons::settings, AppIcons::settings, showSettings, iconSize)) {
		showSettings = !showSettings;
		Config::cfg.widgets.settings = showSettings;
	}
	renderNavtooltip("Settings", showSettings);

	/*if (ToggleIconButtonWidget::render(AppIcons::stream, AppIcons::stream, showStream, iconSize)) {
		showStream = !showStream;
		Config::cfg.widgets.stream = showStream;
	}
	renderNavtooltip("Streaming", showStream);*/
	if (IconButton::render(AppIcons::overlay, AppColors::panelText, iconSize)) {
		if (!OverlayService::instance().isRunning()) {
			OverlayService::instance().start();
		}
		OverlayService::instance().openMenu();
	}
	TitleTooltipWidget::render("Overlay", "Shows the overlay settings window.");

	if (Config::cfg.general.devMode) {
		if (ToggleIconButtonWidget::render(AppIcons::developer, AppIcons::developer, showDevTools, iconSize)) {
			showDevTools = !showDevTools;
			Config::cfg.widgets.devTools = showDevTools;
		}
		renderNavtooltip("Developer Tools", showDevTools);
	}
	else {

		if (IconButton::render(AppIcons::info, AppColors::panelText, iconSize)) {
			//const wchar_t* link = L"https://github.com/soda-arcade/smash-soda/wiki";
			// PathHelper::getCurrentPath() to open "docs/index.html" (not the github wiki)

			const char* curPath = PathHelper::GetCurrentPath().c_str();
			char docsPath[256];
			sprintf(docsPath, "%s\\docs\\index.html", curPath);

			wchar_t link[256];
			mbstowcs(link, docsPath, 256);

			ShellExecute(0, 0, link, 0, 0, SW_SHOW);
		}
		TitleTooltipWidget::render("Overlay", "Shows the help guide.");

	}


	/*if (IconButton::render(AppIcons::logoff, AppColors::primary, iconSize))
	{
		if (!hosting.isRunning())
		{
			showLogin = !showLogin;
		}
	}
	TitleTooltipWidget::render("Log off", "Go back to log in screen.\n\n * Stop streaming before trying to log out.");*/

	ImGui::End();

	ImGui::PopStyleVar();
	ImGui::PopStyleVar();
	//ImGui::PopStyleVar();
	//ImGui::PopStyleVar();
}

void NavBar::renderNavtooltip(const char* title, bool& show)
{
	TitleTooltipWidget::render(title, show ? "Window is visible." : "Window is hidden.");
}
