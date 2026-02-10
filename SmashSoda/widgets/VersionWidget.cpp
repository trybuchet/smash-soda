#include "VersionWidget.h"

// Constructor
VersionWidget::VersionWidget() {
	version = Cache::cache.version;
	latestVersion = "";
	changeLog = "";
	error = "";
    strcpy_s(_email, "");
    strcpy_s(_password, "");
    strcpy_s(_2fa, "");
}

bool VersionWidget::render() {

    Theme* theme = ThemeController::getInstance().getActiveTheme();

    static ImVec2 res;
    static ImVec2 cursor;

    res = ImGui::GetMainViewport()->Size;
    cursor = ImGui::GetCursorPos();

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8,8));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0);
    ImGui::PushStyleVar(ImGuiStyleVar_PopupRounding, 0);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0,0,0,0));

    ImGui::SetNextWindowPos(ImVec2(10, res.y - 42));
    ImGui::SetNextWindowSize(ImVec2(240, 52));
    ImGui::PushFont(AppFonts::label);
    ImGui::Begin("##Version", (bool*)0, ImGuiWindowFlags_NoDecoration);

    ImGui::PushStyleColor(ImGuiCol_Text, theme->formLabel);
    ImGui::Text("Version ");
    ImGui::PopStyleColor();

    ImGui::SameLine();

    ImGui::PushStyleColor(ImGuiCol_Text, theme->text);
    ImGui::Text(version.c_str());
    ImGui::PopStyleColor();

    ImGui::SameLine();
    ImGui::Indent(100.0);
    ImGui::PushStyleColor(ImGuiCol_Text, theme->formLabel);
    ImGui::Text("FPS ");
    ImGui::PopStyleColor();

    ImGui::SameLine();

    ImGui::PushStyleColor(ImGuiCol_Text, theme->text);
    ImGui::Text("%.0f", ImGui::GetIO().Framerate);
    ImGui::PopStyleColor();
    ImGui::Unindent(120.0);
    ImGui::End();

    //ImGui::PopStyleColor();
    //ImGui::PopStyleColor();
    //ImGui::PopStyleColor();
    ImGui::PopStyleColor();
    ImGui::PopStyleVar();
    ImGui::PopStyleVar();
    ImGui::PopStyleVar();
    ImGui::PopStyleVar();

    return true;
}

/// <summary>
/// Login window for Soda Arcade
/// </summary>
/// <returns></returns>
bool VersionWidget::renderLoginWindow() {

    Theme* theme = ThemeController::getInstance().getActiveTheme();

    static ImVec2 res;
    size = ImVec2(400, 564);

    res = ImGui::GetMainViewport()->Size;

    ImGui::SetNextWindowPos(ImVec2(
        (res.x - size.x) * 0.5f,
        (res.y - size.y) * 0.5f
    ));
    ImGui::SetNextWindowSize(size);
    ImGui::SetNextWindowFocus();
    ImGui::PushStyleColor(ImGuiCol_TitleBg, theme->panelTitleBarActive);
    ImGui::PushStyleColor(ImGuiCol_Text, theme->panelTitleBarActiveText);
    ImGui::PushStyleColor(ImGuiCol_Border, theme->panelBorderActive);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::PushFont(AppFonts::title);
    ImGui::Begin("Soda Arcade###Soda Arcade Login Window", (bool*)0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse);
    size = ImGui::GetContentRegionAvail();
    pos = ImGui::GetWindowPos();
    isTitleFocused = ImGui::IsWindowFocused();

    startBody(true);

        // Center image
	    ImGui::SetCursorPosX(ImGui::GetWindowWidth() * 0.5f - 100);
        ImGui::Image(AppIcons::sodaArcadeLogo, ImVec2(200, 46));

        // Text with wrap
        if (error.empty()) {
            elParagraph("Soda Arcade is a free service for advertising your Parsec room. You will need to create a account to use this service.");
        } else {
			ImGui::PushStyleColor(ImGuiCol_Text, AppColors::negative);
			ImGui::TextWrapped(error.c_str());
            ImGui::PopStyleColor();
            ImGui::Dummy(ImVec2(0, 10));
		}

	    ImGui::SetCursorPosX(ImGui::GetWindowWidth() * 0.5f - 112);

        if (elBtnSecondary("I don't want to use Soda Arcade")) {
		    Config::cfg.arcade.showLogin = false;
            Config::cfg.room.privateRoom = true;
		    Config::cfg.Save();
        }

	    ImGui::Dummy(ImVec2(0, 10));

        if (elText("E-Mail Address", _email, "The e-mail address you signed up to Soda Arcade with.")) {

        }

        if (elPassword("Password", _password, "The password you signed up to Soda Arcade with.")) {

        }

        if (elText("2FA Code", _2fa, "2FA code if you have it enabled on your account.")) {

        }
    endBody();

    startFooter();

        if (!_isLoginLocked) {
            if (elBtn("Login")) {
                _isLoginLocked = true;
                _loginThread = thread([&]() {

                    error = "";

                    std::string email = _email;
                    std::string password = _password;
                    std::string twoFactor = _2fa;

                    if (Arcade::instance.login(email, password, twoFactor)) {
                        Config::cfg.arcade.showLogin = false;
                        Config::cfg.Save();
                    } else {
                        error = "Invalid e-mail or password.";
                        _showError = true;
                    }

                    _isLoginLocked = false;
                    _loginThread.detach();
                });

            }
            ImGui::SameLine();
            ImGui::Indent(65);
            if (elBtnSecondary("Create account")) {
                const wchar_t* link = L"https://soda-arcade.com/register";
                ShellExecute(0, 0, link, 0, 0, SW_SHOW);
            }
            ImGui::Unindent(65);
        } else {
            static ImVec2 cursor;
            cursor = ImGui::GetCursorPos();
            cursor.x += 164;
            cursor.y -= 10;
            ImGui::SetCursorPos(cursor);
            LoadingRingWidget::render();
		}

    endFooter();

    ImGui::End();
    ImGui::PopStyleColor();
    ImGui::PopStyleColor();
    ImGui::PopFont();
    ImGui::PopStyleVar();

    return true;

}

/// <summary>
/// Renders the overlay download window
/// </summary>
/// <returns></returns>
bool VersionWidget::renderDownloadWindow() {

    Theme* theme = ThemeController::getInstance().getActiveTheme();
    ImGui::PushFont(AppFonts::title);
    ImGui::PushStyleColor(ImGuiCol_Text, theme->panelText);

    static ImVec2 res;
    static ImVec2 size = ImVec2(400, 200);

    res = ImGui::GetMainViewport()->Size;

    ImGui::SetNextWindowPos(ImVec2(
        (res.x - size.x) * 0.5f,
        (res.y - size.y) * 0.5f
    ));
    ImGui::SetNextWindowSize(size);
    ImGui::SetNextWindowFocus();
    ImGui::Begin("Update Overlay###Overlay Download Window", (bool*)0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse);
    ImGui::PushFont(AppFonts::input);
    ImGui::PushStyleColor(ImGuiCol_Text, theme->formInputText);
    AppColors::pushButtonSolid();

    // Text with wrap
    ImGui::PushFont(AppFonts::input);
    ImGui::PushStyleColor(ImGuiCol_Text, theme->formLabel);
    ImGui::TextWrapped("The overlay files need to be updated. Delete the contents of the overlay folder wherever you have Smash Soda installed, and download the new files.");
    ImGui::PopStyleColor();
    ImGui::PopFont();

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::SetCursorPosX(ImGui::GetWindowWidth() * 0.5f - 35);
    if (ImGui::Button("Download")) {
        const wchar_t* link = L"https://github.com/MickeyUK/smash-soda-overlay/releases/latest";
        ShellExecute(0, 0, link, 0, 0, SW_SHOW);
        Config::cfg.overlay.update = false;
        Config::cfg.Save();
        showDownload = false;
    }

    ImGui::SetCursorPosX(ImGui::GetWindowWidth() * 0.5f - 85);
    if (ImGui::Button("I've already downloaded")) {
        showDownload = false;
        Config::cfg.overlay.update = false;
        Config::cfg.Save();
    }

    AppColors::popButton();
    ImGui::PopStyleColor();
    ImGui::PopFont();
    ImGui::End();
    ImGui::PopStyleColor();
    ImGui::PopFont();

    return true;

}

/// <summary>
/// Renders the update log window
/// </summary>
/// <returns></returns>
bool VersionWidget::renderUpdateWindow() {

    Theme* theme = ThemeController::getInstance().getActiveTheme();
    ImGui::PushFont(AppFonts::title);
    ImGui::PushStyleColor(ImGuiCol_Text, theme->panelText);

    static ImVec2 res;
    static ImVec2 size = ImVec2(400, 500);

    res = ImGui::GetMainViewport()->Size;

    ImGui::SetNextWindowPos(ImVec2(
        (res.x - size.x) * 0.5f,
        (res.y - size.y) * 0.5f
    ));
    ImGui::SetNextWindowSize(size);
    ImGui::SetNextWindowFocus();
    ImGui::Begin("Update Available###Update Window", (bool*)0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse);
    ImGui::PushFont(AppFonts::input);
    ImGui::PushStyleColor(ImGuiCol_Text, theme->formInputText);

    ImGui::PushFont(AppFonts::input);
    ImGui::PushStyleColor(ImGuiCol_Text, theme->positive);
	ImGui::Text("Version: ");
    ImGui::SameLine();
    ImGui::Text(Cache::cache.update.version.c_str());
    ImGui::PopStyleColor();
    ImGui::PopFont();

    // Text with wrap
    ImGui::PushFont(AppFonts::input);
    ImGui::PushStyleColor(ImGuiCol_Text, theme->formLabel);
	ImGui::TextWrapped("A new version of Smash Soda is available. Please close this, pull the latest commit and rebuild!");
    ImGui::PopStyleColor();
    ImGui::PopFont();
    
    ImGui::Spacing();
	ImGui::Separator();
    ImGui::Spacing();
    

    ImGui::Markdown(Cache::cache.update.notes.c_str(), Cache::cache.update.notes.length(), mdConfig);

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    AppColors::pushButtonSolid();
    
    // if (!Cache::cache.update.critical) {
    //     ImGui::SetCursorPosX(ImGui::GetWindowWidth() * 0.5f - 50);
    //     if (ImGui::Button("Close")) {
    //         showUpdate = false;
    //     }
    //     AppColors::popButton();
    // }

    ImGui::PopStyleColor();
    ImGui::PopFont();
    ImGui::End();
    ImGui::PopStyleColor();
    ImGui::PopFont();

    return true;

}


