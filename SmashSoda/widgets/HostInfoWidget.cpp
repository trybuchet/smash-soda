#include "HostInfoWidget.h"

HostInfoWidget::HostInfoWidget(Hosting& hosting)
	: _hosting(hosting)
{
	_loginTime = Clock::now();
}

void HostInfoWidget::renderArcade() {

	Theme* theme = ThemeController::getInstance().getActiveTheme();

	static ImVec2 windowSize = ImVec2(250, 64);
	static ImVec2 windowPos = ImVec2(0, 0);
	static ImVec2 padding = ImVec2(8, 6);
	static ImVec2 viewportSize;
	viewportSize = ImGui::GetMainViewport()->GetCenter();
	viewportSize.x *= 2;
	viewportSize.y *= 2;

	windowPos.x = viewportSize.x - windowSize.x - 10;
	windowPos.y = (viewportSize.y - windowSize.y) - 78;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 6));

	ImGui::Begin(
		"##ArcadeInfoWidget", (bool*)0,
		ImGuiWindowFlags_NoDecoration |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoNav |
		ImGuiWindowFlags_NoBringToFrontOnFocus
	);

	ImGui::SetWindowSize(windowSize);
	ImGui::SetWindowPos(windowPos);

	ImGui::BeginGroup();
		ImGui::Image(AppIcons::arcadeIcon, ImVec2(48, 48));
		ImGui::SameLine();

		// Move cursor UP a bit
		static ImVec2 cursor;
		cursor = ImGui::GetCursorPos();
		cursor.y -= 2;
		ImGui::SetCursorPos(cursor);

		ImGui::BeginGroup();
			if (!Arcade::instance.credentials.token.empty() &&
			!Arcade::instance.credentials.username.empty()) {
				elLabel(Arcade::instance.credentials.username);

				ImGui::PushStyleColor(ImGuiCol_Button, theme->buttonSecondary);
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, theme->buttonSecondaryActive);
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, theme->buttonSecondaryHovered);
				ImGui::PushStyleColor(ImGuiCol_Text, theme->buttonSecondaryText);

				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 2));
				if (ImGui::Button("Logout")) {
					Arcade::instance.logout();
				}

				ImGui::PopStyleColor();
				ImGui::PopStyleColor();
				ImGui::PopStyleColor();
				ImGui::PopStyleColor();
				ImGui::PopStyleVar();
				if (ImGui::IsItemHovered()) ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

			}
			else {
				ImGui::PushStyleColor(ImGuiCol_Text, theme->textMuted);
				ImGui::Text("Not logged in");
				ImGui::PopStyleColor();

				ImGui::PushStyleColor(ImGuiCol_Button, theme->buttonSecondary);
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, theme->buttonSecondaryActive);
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, theme->buttonSecondaryHovered);
				ImGui::PushStyleColor(ImGuiCol_Text, theme->buttonSecondaryText);

				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 2));
				if (ImGui::Button("Login")) {
					Config::cfg.arcade.showLogin = true;
				}

				ImGui::PopStyleColor();
				ImGui::PopStyleColor();
				ImGui::PopStyleColor();
				ImGui::PopStyleColor();
				ImGui::PopStyleVar();
				if (ImGui::IsItemHovered()) ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

			}	
		ImGui::EndGroup();
	ImGui::EndGroup();

}

void HostInfoWidget::render() {

	Theme* theme = ThemeController::getInstance().getActiveTheme();

	static bool isSessionExpired = false;
	isSessionExpired = (_hosting.getHost().status == Guest::Status::EXPIRED);

	static ImVec2 windowSize = ImVec2(250, 60);
	static ImVec2 windowPos = ImVec2(0, 0);
	static ImVec2 padding = ImVec2(8, 6);
	static ImVec2 viewportSize;
	viewportSize = ImGui::GetMainViewport()->GetCenter();
	viewportSize.x *= 2;
	viewportSize.y *= 2;
	windowPos.x = viewportSize.x - windowSize.x - 10;
	windowPos.y = viewportSize.y - windowSize.y - 10;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 6));

	ImGui::Begin(
		"##HostInfoWidget", (bool*)0,
		ImGuiWindowFlags_NoDecoration |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoNav |
		ImGuiWindowFlags_NoBringToFrontOnFocus
	);

	ImGui::SetWindowSize(windowSize);
	ImGui::SetWindowPos(windowPos);

	static ParsecSession& session = _hosting.getSession();

	if (session.type == SessionCache::SessionType::THIRD) {
		static uint32_t remainingTime;
		static float fill;

		if (session.getRemainingTime() > 0)
		{
			remainingTime = session.getRemainingTime();
			fill = 1.0f - ((float)(remainingTime) / session.getLifespan());
		}
		else if (session.isValid() && !session.isUpdating())
		{
			_hosting.fetchAccountData();
		}


		static float radius = 20;
		static float thickness = radius * 0.5f;
		
		ImGui::BeginGroup();

		static ImVec2 cursor;
		cursor = ImGui::GetCursorPos();
		cursor.y += 2;
		ImGui::SetCursorPosY(cursor.y);

		ProgressCircularWidget::render(
			radius, thickness,
			fill, 0.0f, 1.0f,
			ImGui::ColorConvertFloat4ToU32(isSessionExpired ? theme->negative : theme->positive)
		);
		if (ImGui::IsItemHovered())
		{
			TitleTooltipWidget::render
			(
				"Session Expiry",
				(
					string() +
					"Parsec Soda tries to predict the expiry time.\n\n" +
					"Next check in:\n" +
					" > " + Timespan::fromSeconds(remainingTime).toString() + "\n\n"
					"Click to force early refresh."
				).c_str()
			);
		}

		ImGui::SetCursorPos(cursor);

		if (ImGui::Button("### Session refresh button", ImVec2(2 * radius, 2 * radius)))
		{
			if (!session.isUpdating())
			{
				static Debouncer debouncer = Debouncer(500, [&]() {
					_hosting.fetchAccountData(true);
				});
				debouncer.start();
			}
		}
		ImGui::EndGroup();

		ImGui::SameLine();
		ImGui::Dummy(ImVec2(2, 0));
		ImGui::SameLine();
	}

	ImGui::BeginGroup();
	if (_hosting.getHost().isValid()) {
		ImGui::Image(AppIcons::parsecIcon, ImVec2(48, 48));
		ImGui::SameLine();

		// Move cursor UP a bit
		static ImVec2 cursor;
		cursor = ImGui::GetCursorPos();
		cursor.y -= 4;
		ImGui::SetCursorPos(cursor);

		ImGui::PushFont(AppFonts::label);
		ImGui::BeginGroup();
			ImGui::PushStyleColor(ImGuiCol_Text, theme->text);
			ImGui::Text("%s", _hosting.getHost().name.c_str());
			ImGui::PopStyleColor();
			ImGui::SameLine();
			ImGui::PushStyleColor(ImGuiCol_Text, theme->textHighlighted);
			ImGui::Text("#%d", _hosting.getHost().userID);
			ImGui::PopStyleColor();

			ImGui::PushStyleColor(ImGuiCol_Button, theme->buttonSecondary);
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, theme->buttonSecondaryActive);
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, theme->buttonSecondaryHovered);
			ImGui::PushStyleColor(ImGuiCol_Text, theme->buttonSecondaryText);

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 2));
			if (ImGui::Button("Logout")) {
				_hosting.getSession().deleteSessionCache();
			}

			ImGui::PopStyleColor();
			ImGui::PopStyleColor();
			ImGui::PopStyleColor();
			ImGui::PopStyleColor();
			ImGui::PopStyleVar();
			if (ImGui::IsItemHovered()) ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

		ImGui::EndGroup();
		ImGui::PopFont();
	}
	else if (isSessionExpired)
	{
		ImGui::PushFont(AppFonts::label);
		ImGui::PushStyleColor(ImGuiCol_Text, theme->formLabel);
		ImGui::TextWrapped("");
		ImGui::PopStyleColor();
		ImGui::PopFont();
		ImGui::PushFont(AppFonts::input);
		ImGui::PushStyleColor(ImGuiCol_Text, theme->negative);
		ImGui::TextWrapped("Session Expired");
		ImGui::PopStyleColor();
		ImGui::PopFont();
		TitleTooltipWidget::render(
			"Parsec Session Expired",
			(
				string() +
				"Parsec Soda recommends you get a new session:" +
				"  1. Stop streaming.\n" +
				"  2. Click the Log off button.\n" +
				"  3. Authenticate again.\n\n" +
				"Until a new session is acquired:\n" +
				" - Your room will not be listed in public Arcade.\n" +
				" - Guests cannot join your room even through the link.\n" +
				" - Only guests already in this room will see your stream."
			).c_str()
		);
	}
	else
	{
		ImGui::PushFont(AppFonts::label);
		ImGui::PushStyleColor(ImGuiCol_Text, theme->formLabel);
		ImGui::TextWrapped("(# ???)\t");
		ImGui::PopStyleColor();
		ImGui::PopFont();
		ImGui::PushFont(AppFonts::input);
		ImGui::PushStyleColor(ImGuiCol_Text, theme->formInputText);
		ImGui::TextWrapped("Unknown Host");
		ImGui::PopStyleColor();
		ImGui::PopFont();
	}
	ImGui::EndGroup();

	ImGui::End();

	ImGui::PopStyleVar();

	renderArcade();
}
