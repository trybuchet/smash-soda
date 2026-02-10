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

    static ImVec2 size;
    size = ImGui::GetContentRegionAvail();
    static ImVec2 cursor;
    Theme* theme = ThemeController::getInstance().getActiveTheme();

    cursor = ImGui::GetCursorPos();
    ImGui::BeginGroup();

    ImGui::Dummy(ImVec2(0, 5));

    if (Config::cfg.hotseat.enabled) {

        // Calculate the maximum to parent height
        float groupMaxHeight = size.y - 10;

        // Begin a scrolling region with the maximum height
        ImGui::BeginChild("ScrollingRegion", ImVec2(0, groupMaxHeight), true, ImGuiWindowFlags_HorizontalScrollbar);

        // For each HotseatUser in Hotseat::instance.users
        for (Hotseat::HotseatUser& user : Hotseat::instance.users) {

            cursor = ImGui::GetCursorPos();

            ImGui::PushFont(AppFonts::input);
            ImGui::PushStyleColor(ImGuiCol_Text, theme->formInputText);
            ImGui::Indent(5);
            ImGui::Text("%s", user.userName.c_str());
            ImGui::Unindent(5);
            ImGui::PopStyleColor();
            ImGui::PopFont();

            ImGui::SameLine();
            ImGui::PushFont(AppFonts::input);
            ImGui::PushStyleColor(ImGuiCol_Text, theme->positive);
            ImGui::Text("#%d", user.userId);
            ImGui::PopStyleColor();
            ImGui::PopFont();

            ImGui::Indent(5);
            ImGui::PushFont(AppFonts::label);
            ImGui::PushStyleColor(ImGuiCol_Text, theme->formLabel);
            //ImGui::Text(user.status.c_str());

            // Get the time remaining for the user (in minutes)
            if (user.stopwatch->isRunning() && !user.cooldown) {
                ImGui::Text("REMAINING ");
                ImGui::SameLine();
                ImGui::PushStyleColor(ImGuiCol_Text, theme->formInputText);
                ImGui::Text(user.stopwatch->getRemainingTime().c_str());
                ImGui::PopStyleColor();
			}
			else {
                if (user.cooldown) {
                    ImGui::PushStyleColor(ImGuiCol_Text, theme->primary);
                    ImGui::Text("COOLDOWN ");
                    ImGui::PopStyleColor();
                    ImGui::SameLine();
                    ImGui::PushStyleColor(ImGuiCol_Text, theme->formInputText);
                    ImGui::Text(Hotseat::instance.getCooldownRemaining(user.userId).c_str());
                    ImGui::PopStyleColor();
				}
				else {
                    ImGui::Text(user.stopwatch->getRemainingTime().c_str());
				}
			}

            ImGui::PopStyleColor();
            ImGui::PopFont();
            ImGui::Unindent(5);

            ImGui::Dummy(ImVec2(0, 5));

        }

        ImGui::EndChild();

    }

    ImGui::EndGroup();
    
}

/// <summary>
/// Render the hotseat settings
/// </summary>
void HotseatWidget::renderSettings() {

    static ImVec2 size;
    size = ImGui::GetContentRegionAvail();
    static ImVec2 cursor;

    ImGui::Dummy(ImVec2(0, 5));

    cursor = ImGui::GetCursorPos();
    ImGui::BeginGroup();

    if (elNumber("Play Time", _playTime, 1, 999,
        "This is how much time a user is allowed to play every reset (in minutes).")) {
        Config::cfg.hotseat.playTime = _playTime;
        _playTime = Config::cfg.hotseat.playTime;
    }

    if (elNumber("Reset Time", _resetTime, _playTime, 999,
		"This is how often play time resets for each user (in minutes).")) {
        Config::cfg.hotseat.resetTime = _resetTime;
        _resetTime = Config::cfg.hotseat.resetTime;
	}

    if (elNumber("Reminder Interval", _reminderInterval, 0, _playTime-1,
        "Print remaining hotseat time in chat every (x) minutes. 0 for no reminders.")) {
        Config::cfg.hotseat.reminderInterval = _reminderInterval;
        _reminderInterval = Config::cfg.hotseat.reminderInterval;
    }

    ImGui::EndGroup();

}


