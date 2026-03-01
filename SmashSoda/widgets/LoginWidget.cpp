#include "LoginWidget.h"
#include "../services/ThemeController.h"

LoginWidget::LoginWidget(Hosting& hosting, HostSettingsWidget& hostSettingsWidget)
	: _hosting(hosting), _isLoginLocked(false), _hostSettingsWidget(hostSettingsWidget)
{
}

void LoginWidget::render(bool& showLogin) {

    float uiScale = ThemeController::getInstance().getUiScale();
    if (uiScale <= 0.0f) {
        uiScale = 1.0f;
    }

    const float loginWindowWidth = 350.0f * uiScale;
    const float loginWindowHeight = 500.0f * uiScale;
    const float logoWidth = 200.0f * uiScale;
    const float logoHeight = 49.0f * uiScale;

    ImGui::PushStyleColor(ImGuiCol_Text, AppColors::panelTitleBarActiveText);  // Focused text color
    ImGui::PushStyleColor(ImGuiCol_Border, AppColors::panelBorderActive);

    // Set the font
    ImGui::PushFont(AppFonts::title);

    // Get the window size
    size = ImGui::GetIO().DisplaySize;

    // Set window position and size constraints
    ImGui::SetNextWindowPos(ImVec2((size.x - loginWindowWidth) * 0.5f, (size.y - loginWindowHeight) * 0.5f));
    ImGui::SetNextWindowSizeConstraints(ImVec2(loginWindowWidth, loginWindowHeight), ImVec2(loginWindowWidth, loginWindowHeight));

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0, 0, 0, 0.2f));

    // Can't collapse the window
    ImGui::Begin("## Parsec Login", (bool*)0, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBringToFrontOnFocus);

    size = ImGui::GetContentRegionAvail();
    pos = ImGui::GetWindowPos();

    // Reset style
    ImGui::PopStyleVar();
    ImGui::PopStyleColor(3);

    startBody(true);

        ImGui::Dummy(ImVec2(0, 10.0f * uiScale));
        ImGui::SetCursorPosX((size.x - logoWidth) * 0.5f);
        ImGui::Image(AppIcons::logo, ImVec2(logoWidth, logoHeight));

        ImGui::Dummy(ImVec2(0, 20.0f * uiScale));

        elParagraph("This app uses the Parsec service for sharing your screen with other players.");

        renderPersonal(loginWindowWidth, showLogin);

    endBody();

    startFooter();

        if (!_isLoginLocked) {

            if (elBtn("Log in##Login Button")) {
                attemptLoginPersonal(showLogin);
            }

            ImGui::SameLine();
            ImGui::Indent(68.0f * uiScale);

            if (elBtnSecondary("Create new account")) {
                ShellExecute(0, 0, L"https://parsec.app/activate/", 0, 0, SW_SHOW);
            }

        } else {
            static ImVec2 cursor;
            cursor = ImGui::GetCursorPos();
            cursor.x += 140.0f * uiScale;
            cursor.y -= 10.0f * uiScale;
            ImGui::SetCursorPos(cursor);
            LoadingRingWidget::render();
        }

    endFooter();

    if (_showError) {
        ImGui::OpenPopup("Login Failed");
        _showError = false;
    }

    if (AlertWidget::render("Login Failed", _sessionError.c_str())) {
        _showError = false;
    }

    ImGui::End();
    ImGui::PopFont();
}

void LoginWidget::renderPersonal(float width, bool& showLogin) {

    if (elText("E-Mail Address", _email)) {}

    if (elPassword("Password", _password)) {}

    if (elText("2FA", _2fa, "The 2 factor authentication code if it is enabled on your account.")) {}

}

void LoginWidget::render3rd(float width)
{

    Theme* theme = ThemeController::getInstance().getActiveTheme();
    ImGui::Text("Auth Code");
    AppFonts::pushSugoiDekai();
    ImGui::SetNextItemWidth(width);
    ImGui::PushStyleColor(ImGuiCol_Border, theme->formInputBorderActive);
    ImGui::PushStyleColor(ImGuiCol_Text, theme->panelText);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
    ImGui::InputText("##Login auth", _auth.userCode, 16, ImGuiInputTextFlags_ReadOnly);
    ImGui::PopStyleVar();
    ImGui::PopStyleColor();
    ImGui::PopStyleColor();
    AppFonts::pop();
    TitleTooltipWidget::render(
        "Authentication Code",
        "Copy and Paste this code to the link in your browser window\n\nIf a browser window does not show automatically, go to:\nhttps://parsec.app/activate/"
    );
}

void LoginWidget::renderCreateAccount(float width) {
    ImGui::SetCursorPosX(width - 166.0f);
    if (elBtnSecondary("Create new account")) {
        ShellExecute(0, 0, L"https://parsec.app/activate/", 0, 0, SW_SHOW);
    }
    if (ImGui::IsItemHovered()) ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
}

void LoginWidget::renderLoginTooltip()
{
    TitleTooltipWidget::render(
        "Parsec Account",
        (
            string () +
            "Login with your Parsec account.\n" +
            "* Parsec Soda does not collect any data from you."
        ).c_str()
    );
}

void LoginWidget::attemptLoginPersonal(bool& showLogin)
{
    if (!_isLoginLocked)
    {
        _isLoginLocked = true;
        LoadingRingWidget::render(true);
        _loginThread = thread([&]() {
            _hosting.getSession().fetchSession(_email, _password, _2fa);

            if (!_hosting.getSession().isValid())
            {
                _sessionError = _hosting.getSession().getSessionError();
                _sessionStatus = _hosting.getSession().getSessionStatus();
                _showError = true;
            }
            else
            {
                showLogin = false;
            }

            _hosting.fetchAccountData();
            _hostSettingsWidget.updateSecretLink();
            _isLoginLocked = false;
            _loginThread.detach();
        });
    }
}

void LoginWidget::attemptLogin3rd(bool& showLogin)
{
    if (!_isLoginLocked)
    {
        _sessionCancelled = false;
        _showCancelButton = false;
        _isLoginLocked = true;
        LoadingRingWidget::render(true);
        _loginThread = thread([&]() {
            _auth = _hosting.getSession().authenticate();

            if (_auth.success)
            {
                string uri = string(_auth.verificationUri) + "/" + _auth.userCode;
                wstring wuri(&uri[0], &uri[uri.size()]);
                ShellExecute(0, 0, wuri.c_str(), 0, 0, SW_SHOW);
            }

            // Minimum auth cooldown
            Sleep((_auth.interval+1) * 1000);
            ParsecSession::SessionStatus status;

            _showCancelButton = true;

            bool done = false;
            while (!done && !_sessionCancelled)
            {
                status = _hosting.getSession().pollSession(_auth);
                switch (status)
                {
                case ParsecSession::SessionStatus::PENDING:
                    Sleep(1500);
                    break;
                case ParsecSession::SessionStatus::APPROVED:
                    done = true;
                    showLogin = false;
                    break;
                case ParsecSession::SessionStatus::INVALID:
                default:
                    done = true;
                    break;
                }
            }

            _hosting.fetchAccountData();
            _hostSettingsWidget.updateSecretLink();
            _isLoginLocked = false;
            _loginThread.detach();
        });
    }
}

void LoginWidget::renderSodaArcadeLogin() {

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
    Theme* theme = ThemeController::getInstance().getActiveTheme();
    ImGui::PushStyleColor(ImGuiCol_Text, theme->panelTitleBarActiveText);  // Focused text color
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
        if (sodaArcadeError.empty()) {
            elParagraph("Soda Arcade is a free service for advertising your Parsec room. You will need to create a account to use this service.");
        } else {
			ImGui::PushStyleColor(ImGuiCol_Text, theme->negative);
			ImGui::TextWrapped(sodaArcadeError.c_str());
            ImGui::PopStyleColor();
            ImGui::Dummy(ImVec2(0, 10.0f * uiScale));
		}

        ImGui::SetCursorPosX(ImGui::GetWindowWidth() * 0.5f - (112.0f * uiScale));

        if (elBtnSecondary("I don't want to use Soda Arcade")) {
		    Cache::cache.showSodaArcadeLogin = false;
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

                    sodaArcadeError = "";

                    std::string email = _email;
                    std::string password = _password;
                    std::string twoFactor = _2fa;

                    if (Arcade::instance.login(email, password, twoFactor)) {
                        Config::cfg.arcade.showLogin = false;
                        Config::cfg.Save();
                    } else {
                        sodaArcadeError = "Invalid e-mail or password.";
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
    ImGui::PopFont();
    ImGui::PopStyleVar();

}
