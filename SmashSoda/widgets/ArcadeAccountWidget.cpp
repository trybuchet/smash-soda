#include "ArcadeAccountWidget.h"
#include "../Hosting.h"
#include <Windows.h>
#include <shellapi.h>
#include <ctime>
#include <cstdio>

extern Hosting g_hosting;

namespace {
	const std::vector<std::pair<std::string, std::string>>& monthOptions() {
		static const std::vector<std::pair<std::string, std::string>> months = {
			{"", "Select..."},
			{"1", "January"}, {"2", "February"}, {"3", "March"}, {"4", "April"},
			{"5", "May"}, {"6", "June"}, {"7", "July"}, {"8", "August"},
			{"9", "September"}, {"10", "October"}, {"11", "November"}, {"12", "December"},
		};
		return months;
	}

	std::vector<std::pair<std::string, std::string>> dayOptions() {
		std::vector<std::pair<std::string, std::string>> days = { {"", "Select..."} };
		for (int d = 1; d <= 31; ++d) days.push_back({ std::to_string(d), std::to_string(d) });
		return days;
	}

	int currentYear() {
		std::time_t now = std::time(nullptr);
		std::tm local{};
		localtime_s(&local, &now);
		return local.tm_year + 1900;
	}

	std::vector<std::pair<std::string, std::string>> yearOptions() {
		std::vector<std::pair<std::string, std::string>> years = { {"", "Select..."} };
		const int year = currentYear();
		for (int y = year; y >= year - 110; --y) years.push_back({ std::to_string(y), std::to_string(y) });
		return years;
	}

	int daysInMonth(int month, int year) {
		static const int days[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
		const bool leap = (year % 4 == 0 && year % 100 != 0) || year % 400 == 0;
		return month == 2 && leap ? 29 : days[month - 1];
	}

	/// Plain bullet line that wraps inside the current window.
	void bullet(const char* text) {
		ImGui::Bullet();
		ImGui::SameLine();
		ImGui::TextWrapped("%s", text);
	}
}

ArcadeAccountWidget::ArcadeAccountWidget() {}

void ArcadeAccountWidget::setError(const std::string& message) {
	std::lock_guard<std::mutex> lock(_errorMutex);
	_error = message;
}

std::string ArcadeAccountWidget::getError() {
	std::lock_guard<std::mutex> lock(_errorMutex);
	return _error;
}

void ArcadeAccountWidget::openWebPage(const std::string& path) {
	const std::string url = Arcade::instance.webUrl(path);
	ShellExecuteA(0, "open", url.c_str(), 0, 0, SW_SHOW);
}

/// Lists the room straight away if hosting is already running with a public room.
void ArcadeAccountWidget::relistRoomIfPublic() {
	if (g_hosting.isRunning() && !Config::cfg.room.privateRoom) {
		Arcade::instance.createPost();
	}
}

bool ArcadeAccountWidget::validDateOfBirth(std::string& isoDate) {
	if (_day.empty() || _month.empty() || _year.empty()) return false;
	const int day = std::stoi(_day);
	const int month = std::stoi(_month);
	const int year = std::stoi(_year);
	if (month < 1 || month > 12 || day < 1 || day > daysInMonth(month, year)) return false;

	std::time_t now = std::time(nullptr);
	std::tm today{};
	localtime_s(&today, &now);
	const int todayKey = (today.tm_year + 1900) * 10000 + (today.tm_mon + 1) * 100 + today.tm_mday;
	if (year * 10000 + month * 100 + day > todayKey) return false;

	char buffer[11];
	std::snprintf(buffer, sizeof(buffer), "%04d-%02d-%02d", year, month, day);
	isoDate = buffer;
	return true;
}

void ArcadeAccountWidget::beginWindow(const char* title, float width, float height) {
	Theme* theme = ThemeController::getInstance().getActiveTheme();
	float uiScale = ThemeController::getInstance().getUiScale();
	if (uiScale <= 0.0f) uiScale = 1.0f;

	size = ImVec2(width * uiScale, height * uiScale);
	const ImVec2 res = ImGui::GetMainViewport()->Size;
	ImGui::SetNextWindowPos(ImVec2((res.x - size.x) * 0.5f, (res.y - size.y) * 0.5f));
	ImGui::SetNextWindowSize(size);
	// Keep this window in front, but never while a dropdown is open: focusing the
	// window every frame closes its popups, so the date selects couldn't open.
	if (!ImGui::IsPopupOpen("", ImGuiPopupFlags_AnyPopup)) {
		ImGui::SetNextWindowFocus();
	}
	ImGui::PushStyleColor(ImGuiCol_TitleBg, theme->panelTitleBarActive);
	ImGui::PushStyleColor(ImGuiCol_Text, theme->panelTitleBarActiveText);
	ImGui::PushStyleColor(ImGuiCol_Border, theme->panelBorderActive);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::PushFont(AppFonts::title);
	// The body child scrolls on its own; the window itself must not (matches the Soda Arcade login window).
	ImGui::Begin(title, (bool*)0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse
		| ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
	size = ImGui::GetContentRegionAvail();
	pos = ImGui::GetWindowPos();
	isTitleFocused = ImGui::IsWindowFocused();
}

void ArcadeAccountWidget::endWindow() {
	ImGui::End();
	ImGui::PopFont();
	ImGui::PopStyleVar();
	ImGui::PopStyleColor(3);
}

// =============================================================================
//  Account setup: date of birth (once) and the current Terms of Service
// =============================================================================
void ArcadeAccountWidget::renderOnboarding() {
	Theme* theme = ThemeController::getInstance().getActiveTheme();
	float uiScale = ThemeController::getInstance().getUiScale();
	if (uiScale <= 0.0f) uiScale = 1.0f;

	const Arcade::AccountState& account = Arcade::instance.account;

	beginWindow("Soda Arcade###Soda Arcade Account Setup", 420.0f, 600.0f);

	startBody(true);

		ImGui::SetCursorPosX(ImGui::GetWindowWidth() * 0.5f - (100.0f * uiScale));
		ImGui::Image(AppIcons::sodaArcadeLogo, ImVec2(200.0f * uiScale, 46.0f * uiScale));
		ImGui::Dummy(ImVec2(0, 8.0f * uiScale));

		elParagraph("Before your room can be listed on Soda Arcade, we need to finish setting up your account.");

		if (account.needsDateOfBirth) {
			elLabel("Date of birth");
			elHelp("You can only enter this once. Contact support if it ever needs correcting.");

			// Day / Month / Year side by side, so the form fits without scrolling.
			if (ImGui::BeginTable("##Soda Arcade DOB", 3, ImGuiTableFlags_SizingStretchProp, ImVec2(size.x - 40.0f * uiScale, 0))) {
				ImGui::TableSetupColumn("Day", ImGuiTableColumnFlags_WidthStretch, 0.8f);
				ImGui::TableSetupColumn("Month", ImGuiTableColumnFlags_WidthStretch, 1.5f);
				ImGui::TableSetupColumn("Year", ImGuiTableColumnFlags_WidthStretch, 1.1f);
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				elSelect("Day", dayOptions(), _day);
				ImGui::TableSetColumnIndex(1);
				elSelect("Month", monthOptions(), _month);
				ImGui::TableSetColumnIndex(2);
				elSelect("Year", yearOptions(), _year);
				ImGui::EndTable();
			}
		}

		if (account.needsTerms) {
			ImGui::Dummy(ImVec2(0, 6.0f * uiScale));
			elParagraph("We've updated our Terms of Service and Privacy Policy. Please read them before continuing.");
			if (elBtnSecondary("Terms of Service")) openWebPage("/terms");
			ImGui::SameLine();
			if (elBtnSecondary("Privacy Policy")) openWebPage("/privacy");
			ImGui::Dummy(ImVec2(0, 6.0f * uiScale));
			elCheckbox("I agree to the Terms of Service and Privacy Policy", _agreeTerms);
		}

		const std::string error = getError();
		if (!error.empty()) {
			ImGui::Dummy(ImVec2(0, 6.0f * uiScale));
			ImGui::PushStyleColor(ImGuiCol_Text, theme->negative);
			ImGui::TextWrapped("%s", error.c_str());
			ImGui::PopStyleColor();
		}

	endBody();

	startFooter();

		if (_busy) {
			ImVec2 cursor = ImGui::GetCursorPos();
			cursor.x += 164.0f * uiScale;
			cursor.y -= 10.0f * uiScale;
			ImGui::SetCursorPos(cursor);
			LoadingRingWidget::render();
		} else {
			std::string isoDate;
			const bool dateOk = !account.needsDateOfBirth || validDateOfBirth(isoDate);
			const bool termsOk = !account.needsTerms || _agreeTerms;

			ImGui::BeginDisabled(!(dateOk && termsOk));
			if (elBtn("Continue")) {
				_busy = true;
				setError("");
				std::thread([this, isoDate]() {
					const Arcade::OnboardingResult result = Arcade::instance.completeOnboarding(isoDate);
					if (result == Arcade::OnboardingResult::Completed) {
						_agreeTerms = false;
						if (!Arcade::instance.account.onboardingRequired) relistRoomIfPublic();
					} else {
						setError(Arcade::instance.error);
					}
					_busy = false;
				}).detach();
			}
			ImGui::EndDisabled();

			ImGui::SameLine();
			if (elBtnSecondary("Log out of Soda Arcade")) {
				Arcade::instance.logout();
				setError("");
			}
		}

	endFooter();

	endWindow();
}

// =============================================================================
//  Hosting rules: shown before the first room is listed (and after rule updates)
// =============================================================================
void ArcadeAccountWidget::renderHostingRules() {
	Theme* theme = ThemeController::getInstance().getActiveTheme();
	float uiScale = ThemeController::getInstance().getUiScale();
	if (uiScale <= 0.0f) uiScale = 1.0f;

	beginWindow("Before you host###Soda Arcade Hosting Rules", 460.0f, 640.0f);

	startBody(true);

		elParagraph("Listing your room on Soda Arcade shares it with other players. Please read these rules first. They summarise our Terms of Service, which apply in full.");

		ImGui::TextColored(ImVec4(0.24f, 0.81f, 0.56f, 1.0f), "You can host");
		bullet("Games you own, or have permission to stream.");
		bullet("Games rated for younger players, in any room.");
		ImGui::Dummy(ImVec2(0, 6.0f * uiScale));

		ImGui::TextColored(ImVec4(0.94f, 0.27f, 0.27f, 1.0f), "You can't host");
		bullet("Pirated or unlicensed games, ROMs or software.");
		bullet("Films, TV shows, music or streams you don't have the rights to share.");
		bullet("Pornography or sexually explicit content.");
		bullet("Hateful, violent or extremist content, harassment or bullying.");
		bullet("Anything illegal, or anything that puts other people at risk.");
		ImGui::Dummy(ImVec2(0, 6.0f * uiScale));

		ImGui::TextColored(ImVec4(0.96f, 0.65f, 0.14f, 1.0f), "Games rated 18");
		ImGui::TextWrapped("If you stream a game rated 18 (PEGI 18, BBFC 18 or equivalent), tick Mature Room (18+) in Host Settings. Only confirmed adults can host mature rooms, and only users who have confirmed they're 18 or over can see or join them on Soda Arcade. Sexually explicit games aren't allowed in any room, even if they're rated 18.");
		ImGui::Dummy(ImVec2(0, 6.0f * uiScale));

		ImGui::TextColored(ImVec4(0.81f, 0.16f, 0.45f, 1.0f), "Your responsibilities as a host");
		bullet("What you stream and share in your room, and your own behaviour. Public rooms can be joined by anyone, and guests are responsible for their own conduct.");
		bullet("Rooms can be reported. We may remove rooms, restrict hosting or suspend accounts that break these rules, and we report illegal content to the authorities.");
		bullet("Don't charge people to join your room, or use it for advertising, without our written permission.");
		ImGui::Dummy(ImVec2(0, 6.0f * uiScale));

		if (elBtnSecondary("Read the full Terms of Service")) openWebPage("/terms");
		ImGui::Dummy(ImVec2(0, 6.0f * uiScale));

		elCheckbox("I've read and agree to follow these hosting rules and the Terms of Service.", _agreeHostingRules);

		const std::string error = getError();
		if (!error.empty()) {
			ImGui::PushStyleColor(ImGuiCol_Text, theme->negative);
			ImGui::TextWrapped("%s", error.c_str());
			ImGui::PopStyleColor();
		}

	endBody();

	startFooter();

		if (_busy) {
			ImVec2 cursor = ImGui::GetCursorPos();
			cursor.x += 184.0f * uiScale;
			cursor.y -= 10.0f * uiScale;
			ImGui::SetCursorPos(cursor);
			LoadingRingWidget::render();
		} else {
			ImGui::BeginDisabled(!_agreeHostingRules);
			if (elBtn("Accept and list my room")) {
				_busy = true;
				setError("");
				std::thread([this]() {
					if (Arcade::instance.acceptHostingTerms()) {
						_agreeHostingRules = false;
						relistRoomIfPublic();
					} else {
						setError(Arcade::instance.error);
					}
					_busy = false;
				}).detach();
			}
			ImGui::EndDisabled();

			ImGui::SameLine();
			if (elBtnSecondary("Not now")) {
				// Keep hosting, just don't list the room on Soda Arcade.
				Arcade::instance.showHostingRules = false;
				setError("");
				g_hosting.logMessage("Your room won't be listed on Soda Arcade until you accept the hosting rules.");
			}
		}

	endFooter();

	endWindow();
}
