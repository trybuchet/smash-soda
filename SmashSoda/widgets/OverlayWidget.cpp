#include "OverlayWidget.h"
#include "../services/ThemeController.h"
#include "../globals/AppFonts.h"
#include "../Hosting.h"
extern Hosting g_hosting;

OverlayWidget::OverlayWidget(Hosting& hosting)
    : _hosting(hosting), _dx11(_hosting.getDX11())
{
    _chat_enabled = Config::cfg.overlay.chat.active;
	_chat_position = Config::cfg.overlay.chat.position;
    _show_history = Config::cfg.overlay.chat.showHistory;

	_pads_enabled = Config::cfg.overlay.gamepads.active;
	_pads_position = Config::cfg.overlay.gamepads.position;

	_guests_enabled = Config::cfg.overlay.guests.active;
	_guests_position = Config::cfg.overlay.guests.position;
    _show_latency = Config::cfg.overlay.guests.showLatency;
}

bool OverlayWidget::render(bool& showWindow) {

    // Widget top
    Theme* theme = ThemeController::getInstance().getActiveTheme();
    ImGui::PushFont(AppFonts::title);
    ImGui::PushStyleColor(ImGuiCol_Text, theme->panelText);
    ImGui::SetNextWindowSizeConstraints(ImVec2(400, 400), ImVec2(800, 900));
    ImGui::Begin("Overlay", &showWindow);
    if (!showWindow) Config::cfg.widgets.overlay = showWindow;
    ImGui::PushFont(AppFonts::input);
    ImGui::PushStyleColor(ImGuiCol_Text, theme->formInputText);
    
    ImVec2 size = ImGui::GetContentRegionAvail();

    static vector<string> screens = _dx11.listScreens();
    static string screenName = (Config::cfg.overlay.monitor < screens.size()) ? screens[Config::cfg.overlay.monitor] : "";

    ImGui::PushFont(AppFonts::label);
    ImGui::PushStyleColor(ImGuiCol_Text, theme->formLabel);
    ImGui::Text("MONITOR");
    ImGui::PopStyleColor();
    ImGui::PopFont();
    ImGui::PushFont(AppFonts::input);
    ImGui::PushStyleColor(ImGuiCol_Text, theme->formInputText);
    ImGui::SetNextItemWidth(size.x);
    if (ImGui::BeginCombo("##Overlay Monitor list", screenName.c_str())) {
        for (UINT i = 0; i < screens.size(); ++i) {
            bool isSelected = (Config::cfg.overlay.monitor == i);
            if (ImGui::Selectable(screens[i].c_str(), isSelected)) {
                Config::cfg.overlay.monitor = i;
                screenName = screens[Config::cfg.overlay.monitor];
                Config::cfg.Save();
            }
            if (isSelected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }
    ImGui::PopStyleColor();
    ImGui::PopFont();
    ImGui::PushFont(AppFonts::label);
    ImGui::PushStyleColor(ImGuiCol_Text, theme->formLabel);
    ImGui::TextWrapped("The monitor where the overlay will be displayed.");
    ImGui::PopStyleColor();
    ImGui::PopFont();

    ImGui::Dummy(ImVec2(0, 20.0f));

    ImGui::PushFont(AppFonts::label);
    ImGui::PushStyleColor(ImGuiCol_Text, theme->formLabel);
    ImGui::Text("OVERLAY THEME");
    ImGui::PopStyleColor();
    ImGui::PopFont();
    ImGui::SetNextItemWidth(size.x);

    std::string defaultTheme = Config::cfg.overlay.theme == "" ? "None" : Config::cfg.overlay.theme;

    if (ImGui::BeginCombo("### Overlay theme combo", defaultTheme.c_str(), ImGuiComboFlags_HeightLarge)) {

        bool isSelected = false;
        if (Config::cfg.overlay.theme == "" || Config::cfg.overlay.theme == "None") {
            isSelected = true;
        }
        if (ImGui::Selectable("None", isSelected)) {
            Config::cfg.overlay.theme = "";
            Config::cfg.Save();
        }
        if (isSelected) {
            ImGui::SetItemDefaultFocus();
        }

        for (size_t i = 0; i < Config::cfg.overlayThemes.size(); ++i) {

            bool isSelected = false;
            if (Config::cfg.overlay.theme == Config::cfg.overlayThemes[i].c_str()) {
                isSelected = true;
            }

            if (ImGui::Selectable(Config::cfg.overlayThemes[i].c_str(), isSelected)) {
                Config::cfg.overlay.theme = Config::cfg.overlayThemes[i].c_str();
                Config::cfg.Save();
            }
            if (isSelected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }
    ImGui::PushFont(AppFonts::label);
    ImGui::PushStyleColor(ImGuiCol_Text, theme->formLabel);
    ImGui::TextWrapped("Custom CSS files loaded from the 'overlay/themes' folder.");
    ImGui::PopStyleColor();
    ImGui::PopFont();

    ImGui::Dummy(ImVec2(0, 20.0f));

    ImGui::PushFont(AppFonts::label);
    ImGui::PushStyleColor(ImGuiCol_Text, theme->positive);
    ImGui::Text("CHAT WIDGET");
    ImGui::PopStyleColor();
    ImGui::PopFont();
    ImGui::Dummy(ImVec2(0, 10.0f));
    ImGui::PushFont(AppFonts::input);
    ImGui::PushStyleColor(ImGuiCol_Text, theme->formInputText);

    if (ImForm::InputCheckbox("Show Chat", _chat_enabled,
		"Show chat window on the overlay.")) {
        Config::cfg.overlay.chat.active = _chat_enabled;
        Config::cfg.Save();
    }

    if (ImForm::InputCheckbox("Show History", _show_history,
        "This allows you to scroll through previous messages with the mouse wheel.")) {
        Config::cfg.overlay.chat.showHistory = _show_history;
        Config::cfg.Save();
    }

    ImGui::PopStyleColor();
    ImGui::PopFont();
    ImGui::PushFont(AppFonts::label);
    ImGui::PushStyleColor(ImGuiCol_Text, theme->formLabel);
    ImGui::Text("POSITION");
    ImGui::PopStyleColor();
    ImGui::PopFont();
    ImGui::SetNextItemWidth(size.x);
    if (ImGui::BeginCombo("### Chat picker combo", _chat_position.c_str(), ImGuiComboFlags_HeightLarge)) {
        for (size_t i = 0; i < 6; ++i) {
            
            bool isSelected = false;
			if (Config::cfg.overlay.chat.position == _position_options[i].c_str()) {
				isSelected = true;
				_chat_position = _position_options[i].c_str();
			}
            
            if (ImGui::Selectable(_position_options[i].c_str(), isSelected)) {
                Config::cfg.overlay.chat.position = _position_options[i].c_str();
                _chat_position = _position_options[i].c_str();
                Config::cfg.Save();
            }
            if (isSelected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }

    ImGui::Dummy(ImVec2(0, 20.0f));

    ImGui::PushFont(AppFonts::label);
    ImGui::PushStyleColor(ImGuiCol_Text, theme->positive);
    ImGui::Text("GAMEPADS WIDGET");
    ImGui::PopStyleColor();
    ImGui::PopFont();
    ImGui::Dummy(ImVec2(0, 10.0f));
    ImGui::PushFont(AppFonts::input);
    ImGui::PushStyleColor(ImGuiCol_Text, theme->formInputText);

    if (ImForm::InputCheckbox("Show Gamepads", _pads_enabled,
        "Show gamepad info on the overlay.")) {
        Config::cfg.overlay.gamepads.active = _pads_enabled;
        Config::cfg.Save();
    }

    ImGui::PopStyleColor();
    ImGui::PopFont();
    ImGui::PushFont(AppFonts::label);
    ImGui::PushStyleColor(ImGuiCol_Text, theme->formLabel);
    ImGui::Text("POSITION");
    ImGui::PopStyleColor();
    ImGui::PopFont();
    ImGui::SetNextItemWidth(size.x);
    if (ImGui::BeginCombo("### Pads picker combo", _pads_position.c_str(), ImGuiComboFlags_HeightLarge)) {
        for (size_t i = 0; i < 6; ++i) {

            bool isSelected = false;
            if (Config::cfg.overlay.gamepads.position == _position_options[i].c_str()) {
                isSelected = true;
                _pads_position = _position_options[i].c_str();
            }

            if (ImGui::Selectable(_position_options[i].c_str(), isSelected)) {
                Config::cfg.overlay.gamepads.position = _position_options[i].c_str();
                _pads_position = _position_options[i].c_str();
                Config::cfg.Save();
            }
            if (isSelected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }

    ImGui::Dummy(ImVec2(0, 20.0f));

    ImGui::PushFont(AppFonts::label);
    ImGui::PushStyleColor(ImGuiCol_Text, theme->positive);
    ImGui::Text("GUESTS WIDGET");
    ImGui::PopStyleColor();
    ImGui::PopFont();
    ImGui::Dummy(ImVec2(0, 10.0f));
    ImGui::PushFont(AppFonts::input);
    ImGui::PushStyleColor(ImGuiCol_Text, theme->formInputText);

    if (ImForm::InputCheckbox("Show Guests", _guests_enabled,
        "Show guest info on the overlay.")) {
        Config::cfg.overlay.guests.active = _guests_enabled;
        Config::cfg.Save();
    }

    if (ImForm::InputCheckbox("Show Latency", _show_latency,
        "Show guests' pings on the guests widget.")) {
        Config::cfg.overlay.guests.showLatency = _show_latency;
        Config::cfg.Save();
    }

    ImGui::PopStyleColor();
    ImGui::PopFont();
    ImGui::PushFont(AppFonts::label);
    ImGui::PushStyleColor(ImGuiCol_Text, theme->formLabel);
    ImGui::Text("POSITION");
    ImGui::PopStyleColor();
    ImGui::PopFont();
    ImGui::SetNextItemWidth(size.x);
    if (ImGui::BeginCombo("### Guests picker combo", _guests_position.c_str(), ImGuiComboFlags_HeightLarge)) {
        for (size_t i = 0; i < 6; ++i) {

            bool isSelected = false;
            if (Config::cfg.overlay.guests.position == _position_options[i].c_str()) {
                isSelected = true;
                _guests_position = _position_options[i].c_str();
            }

            if (ImGui::Selectable(_position_options[i].c_str(), isSelected)) {
                Config::cfg.overlay.guests.position = _position_options[i].c_str();
                _guests_position = _position_options[i].c_str();
                Config::cfg.Save();
            }
            if (isSelected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }

    // Widget bottom
    ImGui::PopStyleColor();
    ImGui::PopFont();
    ImGui::End();
    ImGui::PopStyleColor();
    ImGui::PopFont();

    return true;

}


