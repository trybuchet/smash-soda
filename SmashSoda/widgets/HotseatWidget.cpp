#include "HotseatWidget.h"
#include "../services/ThemeController.h"
#include "../globals/AppFonts.h"
#include "../Hosting.h"
#include "../services/Hotseat.h"
extern Hosting g_hosting;

HotseatWidget::HotseatWidget(Hosting& hosting)
    : _hosting(hosting)
{
	_playTime = Config::cfg.hotseat.playTime;
	_resetTime = Config::cfg.hotseat.resetTime;
    _reminderInterval = Config::cfg.hotseat.reminderInterval;
}

bool HotseatWidget::render(bool& showWindow) {

    startWidget("Hotseat", showWindow, 782, 10, 400, 800, 350, 400);
    
    std::vector<Tab> tabs = {
        {"Users",    [&](){ renderOverview(); }},
        {"Settings", [&](){ renderSettings(); }},
    };
    startTabs( tabs );
    endTabs();

    endWidget();

    return true;

}

/// <summary>
/// Render users in the hotseat
/// </summary>
void HotseatWidget::renderOverview() {

    ImVec2 size = ImGui::GetContentRegionAvail();
    Theme* theme = ThemeController::getInstance().getActiveTheme();

    ImGui::Dummy(ImVec2(0, 5));

    if (Config::cfg.hotseat.enabled) {

        // Fill remaining tab body area without drawing an extra border.
        ImGui::BeginChild("ScrollingRegion", ImVec2(0, size.y - 10), false);

        // For each HotseatUser in Hotseat::instance.users
        for (Hotseat::HotseatUser& user : Hotseat::instance.users) {

            ImGui::PushFont(AppFonts::input);
            ImGui::PushStyleColor(ImGuiCol_Text, theme->formInputText);
            ImGui::Text("%s", user.userName.c_str());
            ImGui::PopStyleColor();
            ImGui::PopFont();

            ImGui::SameLine();
            ImGui::PushFont(AppFonts::input);
            ImGui::PushStyleColor(ImGuiCol_Text, theme->positive);
            ImGui::Text("#%d", user.userId);
            ImGui::PopStyleColor();
            ImGui::PopFont();

            ImGui::PushFont(AppFonts::label);
            ImGui::PushStyleColor(ImGuiCol_Text, theme->formLabel);

            if (user.cooldown) {
                ImGui::PushStyleColor(ImGuiCol_Text, theme->primary);
                ImGui::Text("RESET IN ");
                ImGui::SameLine();
                ImGui::PopStyleColor();
                ImGui::PushStyleColor(ImGuiCol_Text, theme->formInputText);
                ImGui::Text("%s", Hotseat::instance.getCooldownRemaining(user.userId).c_str());
                ImGui::PopStyleColor();
            }
            else if (user.inSeat && user.stopwatch->isRunning()) {
                ImGui::Text("REMAINING ");
                ImGui::SameLine();
                ImGui::PushStyleColor(ImGuiCol_Text, theme->formInputText);
                ImGui::Text("%s", user.stopwatch->getRemainingTime().c_str());
                ImGui::PopStyleColor();
            }
            else {
                ImGui::PushStyleColor(ImGuiCol_Text, theme->formHelpText);
                ImGui::Text("PAUSED ");
                ImGui::PopStyleColor();
                ImGui::SameLine();
                ImGui::PushStyleColor(ImGuiCol_Text, theme->formInputText);
                ImGui::Text("%s", user.stopwatch->getRemainingTime().c_str());
                ImGui::PopStyleColor();
            }

            ImGui::PopStyleColor();
            ImGui::PopFont();

            ImGui::Dummy(ImVec2(0, 5));

        }

        ImGui::EndChild();

    }
    
}

/// <summary>
/// Render the hotseat settings
/// </summary>
void HotseatWidget::renderSettings() {

    ImGui::Dummy(ImVec2(0, 5));

    if (elNumber("Play Time", _playTime, 1, 999,
        "This is how much time a user is allowed to play every reset (in minutes).")) {
        const int previousPlayTime = Config::cfg.hotseat.playTime;
        Config::cfg.hotseat.playTime = _playTime;
        _playTime = Config::cfg.hotseat.playTime;
        Config::cfg.Save();
        Hotseat::instance.applySettingsDelta(
            Config::cfg.hotseat.playTime - previousPlayTime,
            0,
            Config::cfg.hotseat.reminderInterval
        );
    }

    if (elNumber("Reset Time", _resetTime, _playTime, 999,
			"This is how often play time resets for each user (in minutes).")) {
        const int previousResetTime = Config::cfg.hotseat.resetTime;
        Config::cfg.hotseat.resetTime = _resetTime;
        _resetTime = Config::cfg.hotseat.resetTime;
        Config::cfg.Save();
        Hotseat::instance.applySettingsDelta(
            0,
            Config::cfg.hotseat.resetTime - previousResetTime,
            Config::cfg.hotseat.reminderInterval
        );
		}

    if (elNumber("Reminder Interval", _reminderInterval, 0, _playTime-1,
        "Print remaining hotseat time in chat every (x) minutes. 0 for no reminders.")) {
        Config::cfg.hotseat.reminderInterval = _reminderInterval;
        _reminderInterval = Config::cfg.hotseat.reminderInterval;
        Config::cfg.Save();
        Hotseat::instance.applySettingsDelta(0, 0, Config::cfg.hotseat.reminderInterval);
    }

}


