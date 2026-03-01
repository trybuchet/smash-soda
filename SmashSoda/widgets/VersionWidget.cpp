#include "VersionWidget.h"
#include <Windows.h>
#include <shellapi.h>

// Constructor
VersionWidget::VersionWidget(function<void()> onSodaArcadeLogin)
    : _onSodaArcadeLogin(onSodaArcadeLogin) {
	version = Cache::cache.version;
	latestVersion = "";
	changeLog = "";
	error = "";
    strcpy_s(_email, "");
    strcpy_s(_password, "");
    strcpy_s(_2fa, "");
}

bool VersionWidget::render() {

    if (_pendingSodaArcadeLoginRefresh.exchange(false) && _onSodaArcadeLogin) {
        _onSodaArcadeLogin();
    }

    Theme* theme = ThemeController::getInstance().getActiveTheme();

    static ImVec2 res;
    res = ImGui::GetMainViewport()->Size;

    float uiScale = ThemeController::getInstance().getUiScale();
    if (uiScale < 1.0f) {
        uiScale = 1.0f;
    }
    const ImVec2 padding = ImVec2(8.0f * uiScale, 8.0f * uiScale);
    const float margin = 10.0f * uiScale;
    const float groupGap = 24.0f * uiScale;

    const std::string versionValue = version.empty() ? "-" : version;
    const std::string fpsValue = std::to_string(static_cast<int>(ImGui::GetIO().Framerate + 0.5f));

    const ImVec2 versionLabelSize = ImGui::CalcTextSize("Version ");
    const ImVec2 versionValueSize = ImGui::CalcTextSize(versionValue.c_str());
    const ImVec2 fpsLabelSize = ImGui::CalcTextSize("FPS ");
    const ImVec2 fpsValueSize = ImGui::CalcTextSize(fpsValue.c_str());

    const float leftGroupWidth = versionLabelSize.x + versionValueSize.x;
    const float rightGroupWidth = fpsLabelSize.x + fpsValueSize.x;
    const float contentWidth = leftGroupWidth + groupGap + rightGroupWidth;
    const float windowWidth = contentWidth + padding.x * 2.0f;
    const float windowHeight = ImGui::GetTextLineHeight() + padding.y * 2.0f;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, padding);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0);
    ImGui::PushStyleVar(ImGuiStyleVar_PopupRounding, 0);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0,0,0,0));

    ImGui::SetNextWindowPos(ImVec2(margin, res.y - windowHeight - margin));
    ImGui::SetNextWindowSize(ImVec2(windowWidth, windowHeight));
    ImGui::PushFont(AppFonts::label);
    ImGui::Begin("##Version", (bool*)0, ImGuiWindowFlags_NoDecoration);

    ImGui::PushStyleColor(ImGuiCol_Text, theme->formLabel);
    ImGui::TextUnformatted("Version ");
    ImGui::PopStyleColor();

    ImGui::SameLine(0.0f, 0.0f);

    ImGui::PushStyleColor(ImGuiCol_Text, theme->text);
    ImGui::TextUnformatted(versionValue.c_str());
    ImGui::PopStyleColor();

    const float rightCursorX = ImGui::GetWindowContentRegionMax().x - rightGroupWidth;
    ImGui::SetCursorPosX(rightCursorX);

    ImGui::PushStyleColor(ImGuiCol_Text, theme->formLabel);
    ImGui::TextUnformatted("FPS ");
    ImGui::PopStyleColor();

    ImGui::SameLine(0.0f, 0.0f);

    ImGui::PushStyleColor(ImGuiCol_Text, theme->text);
    ImGui::TextUnformatted(fpsValue.c_str());
    ImGui::PopStyleColor();

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
    float uiScale = ThemeController::getInstance().getUiScale();
    if (uiScale <= 0.0f) {
        uiScale = 1.0f;
    }

    static ImVec2 res;
    size = ImVec2(400.0f * uiScale, 564.0f * uiScale);

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
	    ImGui::SetCursorPosX(ImGui::GetWindowWidth() * 0.5f - (100.0f * uiScale));
        ImGui::Image(AppIcons::sodaArcadeLogo, ImVec2(200.0f * uiScale, 46.0f * uiScale));

        // Text with wrap
        if (error.empty()) {
            elParagraph("Soda Arcade is a free service for advertising your Parsec room. You will need to create a account to use this service.");
        } else {
			ImGui::PushStyleColor(ImGuiCol_Text, AppColors::negative);
			ImGui::TextWrapped(error.c_str());
            ImGui::PopStyleColor();
            ImGui::Dummy(ImVec2(0, 10.0f * uiScale));
		}

        ImGui::SetCursorPosX(ImGui::GetWindowWidth() * 0.5f - (112.0f * uiScale));

        if (elBtnSecondary("I don't want to use Soda Arcade")) {
		    Config::cfg.arcade.showLogin = false;
            Config::cfg.room.privateRoom = true;
		    Config::cfg.Save();
        }

        ImGui::Dummy(ImVec2(0, 10.0f * uiScale));

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
                        _pendingSodaArcadeLoginRefresh = true;
                    } else {
                        error = "Invalid e-mail or password.";
                        _showError = true;
                    }

                    _isLoginLocked = false;
                    _loginThread.detach();
                });

            }
            ImGui::SameLine();
            ImGui::Indent(65.0f * uiScale);
            if (elBtnSecondary("Create account")) {
                const wchar_t* link = L"https://soda-arcade.com/register";
                ShellExecute(0, 0, link, 0, 0, SW_SHOW);
            }
            ImGui::Unindent(65.0f * uiScale);
        } else {
            static ImVec2 cursor;
            cursor = ImGui::GetCursorPos();
            cursor.x += 164.0f * uiScale;
            cursor.y -= 10.0f * uiScale;
            ImGui::SetCursorPos(cursor);
            LoadingRingWidget::render();
		}

    endFooter();

    ImGui::End();
    ImGui::PopStyleColor();
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

    bool startUpdate = false;
    static std::string updateError = "";

    static ImVec2 res;
    static ImVec2 size = ImVec2(400, 500);

    res = ImGui::GetMainViewport()->Size;

    ImGui::SetNextWindowPos(ImVec2(
        (res.x - size.x) * 0.5f,
        (res.y - size.y) * 0.5f
    ));
    ImGui::SetNextWindowSize(size);
    ImGui::SetNextWindowFocus();
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::Begin("Update Available###Update Window", (bool*)0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse);
    this->size = ImGui::GetContentRegionAvail();
    pos = ImGui::GetWindowPos();
    isTitleFocused = ImGui::IsWindowFocused();

    startBody(true);

    ImGui::PushFont(AppFonts::input);

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

    if (!updateError.empty()) {
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();
        ImGui::PushStyleColor(ImGuiCol_Text, AppColors::negative);
        ImGui::TextWrapped("%s", updateError.c_str());
        ImGui::PopStyleColor();
    }

    ImGui::PopFont();

    endBody();

    startFooter();

    AppColors::pushButtonSolid();

    if (elBtn("Update")) {
        updateError = "";

        char exePath[MAX_PATH] = {};
        GetModuleFileNameA(NULL, exePath, MAX_PATH);

        std::string appDir = exePath;
        size_t sep = appDir.find_last_of("\\/");
        if (sep != std::string::npos) {
            appDir = appDir.substr(0, sep);
            std::string updaterPath = appDir + "\\updater.bat";

            if (MTY_FileExists(updaterPath.c_str())) {
                std::string updaterCmdArgs = "/c \"" + updaterPath + "\"";
                if (!Cache::cache.update.overlay.empty()) {
                    updaterCmdArgs += " " + Cache::cache.update.overlay;
                }

                HINSTANCE launched = ShellExecuteA(NULL, "open", "cmd.exe", updaterCmdArgs.c_str(), appDir.c_str(), SW_SHOWDEFAULT);
                if ((INT_PTR)launched > 32) {
                    startUpdate = true;
                } else {
                    updateError = "Failed to start updater.bat";
                }
            } else {
                updateError = "updater.bat not found next to SmashSoda.exe";
            }
        } else {
            updateError = "Could not resolve app folder";
        }
    }

    ImGui::SameLine(0.0f, 16.0f);
    if (elBtnSecondary("Dismiss")) {
        showUpdate = false;
        updateError = "";
    }

    AppColors::popButton();

    endFooter();

    ImGui::End();
    ImGui::PopStyleVar();
    ImGui::PopStyleColor();
    ImGui::PopFont();

    return startUpdate;

}


