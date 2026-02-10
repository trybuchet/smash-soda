#include "DeveloperWidget.h"
#include "../Hosting.h"
extern Hosting g_hosting;
#include "../services/ThemeController.h"
#include "../globals/AppFonts.h"

DeveloperWidget::DeveloperWidget(Hosting& hosting)
    : _hosting(hosting) {

    try {
        strcpy_s(_stagingDomain, Config::cfg.developer.stagingDomain.c_str());
    }
    catch (const std::exception&) {
        try
        {
            strcpy_s(_stagingDomain, "");
        }
        catch (const std::exception&) {}
    }

    try {
		strcpy_s(_devDomain, Config::cfg.developer.devDomain.c_str());
	}
	catch (const std::exception&) {
		try
		{
			strcpy_s(_devDomain, "");
		}
		catch (const std::exception&) {}
	}

    _useDevDomain = Config::cfg.developer.useDevDomain;
    _useStagingDomain = Config::cfg.developer.useStagingDomain;

}

bool DeveloperWidget::render(bool& showWindow) {

    Theme* theme = ThemeController::getInstance().getActiveTheme();
    ImGui::PushFont(AppFonts::title);
    ImGui::PushStyleColor(ImGuiCol_Text, theme->panelText);
    ImGui::SetNextWindowSizeConstraints(ImVec2(400, 400), ImVec2(800, 900));
    ImGui::Begin("Developer", &showWindow);
    ImGui::PushFont(AppFonts::label);
    ImGui::PushStyleColor(ImGuiCol_Text, theme->formLabel);
    ImGui::TextWrapped("DO NOT MODIFY ANY OF THESE UNLESS YOU KNOW WHAT YOU ARE DOING!");
    ImGui::PopStyleColor();
    ImGui::PopFont();

    ImGui::Dummy(ImVec2(0, 10.0f));

    ImGui::PushFont(AppFonts::input);
    ImGui::PushStyleColor(ImGuiCol_Text, theme->formInputText);
    ImVec2 size = ImGui::GetContentRegionAvail();

    if (ImForm::InputCheckbox("Use Dev Domain", _useDevDomain)) {
        Config::cfg.developer.useDevDomain = _useDevDomain;
        Config::cfg.Save();
    }

    if (ImForm::InputText("DEV DOMAIN", _devDomain, 
        "Don't include protocol. Expects unsecured connection has Soketi won't work with self signed SSL certificates.")) {
        Config::cfg.developer.devDomain = _devDomain;
        Config::cfg.Save();
    }

    if (ImForm::InputCheckbox("Use staging Domain", _useStagingDomain,
        "Don't include protocol. For testing in a live setting.")) {
        Config::cfg.developer.useStagingDomain = _useStagingDomain;
        Config::cfg.Save();
    }

    if (ImForm::InputText("STAGING DOMAIN", _stagingDomain)) {
        Config::cfg.developer.stagingDomain = _stagingDomain;
        Config::cfg.Save();
    }

    ImGui::BeginGroup();
    AppColors::pushButtonSecondary();
    if (ImGui::Button("Add Fake Guest")) {
        if (!_hosting.isRunning()) {
            _hosting.addFakeGuests(1);
        }
    }
    AppColors::popButton();
    ImGui::PushFont(AppFonts::label);
    ImGui::PushStyleColor(ImGuiCol_Text, theme->formLabel);
    ImGui::TextWrapped("This can only be used when not hosting. For testing commands.");
    ImGui::PopStyleColor();
    ImGui::PopFont();
    ImGui::EndGroup();

    ImGui::Dummy(ImVec2(0, 10.0f));

    ImGui::BeginGroup();
    AppColors::pushButtonSecondary();
    if (ImGui::Button("Toggle Hotseat Thread")) {
        if (!_hosting.getHotseat().running) {
            Config::cfg.hotseat.enabled = true;
			_hosting.getHotseat().start();
		}
        else {
            Config::cfg.hotseat.enabled = false;
			_hosting.getHotseat().stop();
		}
    }
    AppColors::popButton();
    ImGui::PushFont(AppFonts::label);
    ImGui::PushStyleColor(ImGuiCol_Text, theme->formLabel);
    ImGui::TextWrapped("This can only be used when not hosting. For testing commands.");
    ImGui::PopStyleColor();
    ImGui::PopFont();
    ImGui::EndGroup();

    ImGui::PopStyleColor();
    ImGui::PopFont();
    ImGui::End();
    ImGui::PopStyleColor();
    ImGui::PopFont();

    return true;

}


