#include "SettingsWidget.h"
#include "../ImGui/imform.h"
#include "../helpers/PathHelper.h"
#include <algorithm>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace {
    struct SfxRow {
        std::string path;
        std::string tag;
        int cooldown = 5;
    };

    static bool LoadSfxRows(std::vector<SfxRow>& rows, std::string& errorMessage) {
        rows.clear();
        errorMessage.clear();

        const std::string configPath = PathHelper::GetConfigPath();
        if (configPath.empty()) {
            errorMessage = "Unable to locate config path.";
            return false;
        }

        const std::string sfxConfigPath = configPath + "sfx.json";
        if (!MTY_FileExists(sfxConfigPath.c_str())) {
            return true;
        }

        try {
            size_t size = 0;
            void* data = MTY_ReadFile(sfxConfigPath.c_str(), &size);
            if (data == nullptr || size == 0) {
                if (data != nullptr) {
                    MTY_Free(data);
                }
                return true;
            }

            std::string content(static_cast<char*>(data), size);
            MTY_Free(data);

            const json root = json::parse(content);
            if (!root.contains("sfx") || !root["sfx"].is_array()) {
                return true;
            }

            for (const auto& item : root["sfx"]) {
                if (!item.is_object()) {
                    continue;
                }

                SfxRow row;
                row.path = item.value("path", "");
                if (row.path.empty()) {
                    continue;
                }

                row.tag = item.value("tag", row.path);
                row.cooldown = item.value("cooldown", 5);
                if (row.cooldown < 0) {
                    row.cooldown = 0;
                }

                rows.push_back(row);
            }

            std::sort(rows.begin(), rows.end(), [](const SfxRow& a, const SfxRow& b) {
                return a.tag < b.tag;
            });
        }
        catch (const std::exception&) {
            errorMessage = "Failed to read sfx.json.";
            return false;
        }

        return true;
    }

    static bool SaveSfxRows(const std::vector<SfxRow>& rows) {
        const std::string configPath = PathHelper::GetConfigPath();
        if (configPath.empty()) {
            return false;
        }

        json out;
        out["sfx"] = json::array();
        for (const auto& row : rows) {
            json entry;
            entry["path"] = row.path;
            entry["tag"] = row.tag.empty() ? row.path : row.tag;
            entry["cooldown"] = row.cooldown < 0 ? 0 : row.cooldown;
            out["sfx"].push_back(entry);
        }

        const std::string sfxConfigPath = configPath + "sfx.json";
        const std::string content = out.dump(4);
        return MTY_WriteTextFile(sfxConfigPath.c_str(), "%s", content.c_str());
    }

    static void BeginTabBody(const char* id) {
        Theme* theme = ThemeController::getInstance().getActiveTheme();
        ImGui::PushStyleColor(ImGuiCol_ChildBg, theme->panelBackground);
        ImGui::PushStyleColor(ImGuiCol_Text, theme->panelText);
        ImGui::BeginChild(id, ImVec2(0.0f, 0.0f), false);
        ImGui::Dummy(ImVec2(0, 10));
    }

    static void EndTabBody() {
        ImGui::EndChild();
        ImGui::PopStyleColor(2);
    }
}

SettingsWidget::SettingsWidget(Hosting& hosting)
    : _hosting(hosting)
{
    _disableGuideButton = Config::cfg.input.disableGuideButton;
    _disableKeyboard = Config::cfg.input.disableKeyboard;
    _latencyLimitEnabled = Config::cfg.room.latencyLimit;
    _latencyLimitThreshold = Config::cfg.room.latencyLimitThreshold;
    _leaderboardEnabled = false;
    _autoIndex = Config::cfg.input.autoIndex;
    _flashWindow = Config::cfg.general.flashWindow;
    _sfxEnabled = Config::cfg.audio.sfxEnabled;
    _hostBonkProof = Config::cfg.chat.hostBonkProof;
    _ipBan = Config::cfg.general.ipBan;
    _parsecLogs = Config::cfg.general.parsecLogs;
    _blockVPN = Config::cfg.general.blockVPN;
    _devMode = Config::cfg.general.devMode;

    _microphoneEnabled = Config::cfg.audio.micEnabled;

    _muteTime = Config::cfg.chat.muteTime;
    _autoMute = Config::cfg.chat.autoMute;
    _autoMuteTime = Config::cfg.chat.autoMuteTime;
    _saveChat = false;

    _hotkeyBB = Config::cfg.general.hotkeyBB;
    _hotkeyLock = Config::cfg.general.hotkeyLock;

    _guestSFX = Config::cfg.permissions.guest.useSFX;
    _guestBB = Config::cfg.permissions.guest.useBB;
    _guestControls = Config::cfg.permissions.guest.changeControls;

    _vipSFX = Config::cfg.permissions.vip.useSFX;
    _vipBB = Config::cfg.permissions.vip.useBB;
    _vipControls = Config::cfg.permissions.vip.changeControls;

    _modSFX = Config::cfg.permissions.moderator.useSFX;
    _modBB = Config::cfg.permissions.moderator.useBB;
    _modControls = Config::cfg.permissions.moderator.changeControls;

    _prependPingLimit = false;
    _countries = Countries();

    _socketEnabled = Config::cfg.socket.enabled;
    _socketPort = Config::cfg.socket.port;

    try {
        strcpy_s(_discord, Config::cfg.chat.discord.c_str());
    }
    catch (const std::exception&) {
        strcpy_s(_discord, "");
    }

    try {
        strcpy_s(_chatbot, Config::cfg.chat.chatbot.c_str());
    }
    catch (const std::exception&) {
        strcpy_s(_chatbot, "");
    }

    try {
        strcpy_s(_welcomeMessage, Config::cfg.chat.welcomeMessage.c_str());
    }
    catch (const std::exception&) {
        strcpy_s(_welcomeMessage, "");
    }
}

bool SettingsWidget::render(bool& showWindow) {
    startWidget("Settings", showWindow, 1142, 10, 800, 900, 350, 400);

    std::vector<Tab> tabs = {
        {"General", [&]() { renderGeneral(); }},
        {"Permissions", [&]() { renderPermissions(); }},
        {"Hotkeys", [&]() { renderHotkeys(); }},
        {"SFX", [&]() { renderSfx(); }},
    };

    startTabs(tabs);
    endTabs();
    endWidget();

    return true;
}

void SettingsWidget::renderGeneral() {
    BeginTabBody("##settings-body-general");

    if (elSelect("Country", _countries.list, Config::cfg.arcade.country,
        "This is used to better match you with other players in your region.")) {
        Config::cfg.Save();
    }

    if (elSelect("Theme", ThemeController::getInstance().getThemeNames(), Config::cfg.general.theme,
        "Change the color scheme of Smash Soda.")) {
        ThemeController::getInstance().applyTheme(Config::cfg.general.theme);
        Config::cfg.Save();
    }

    if (elText("Chatbot Name", _chatbot,
        "Can give the ChatBot a silly name if you want!")) {
        Config::cfg.chat.chatbot = _chatbot;
        Config::cfg.Save();
        Config::cfg.chatbotName = "[" + Config::cfg.chat.chatbot + "] ";
    }

    if (elTextArea("Welcome Message", _welcomeMessage,
        "Joining guests will see this message. Type _PLAYER_ to insert the guest's name in the message.")) {
        Config::cfg.chat.welcomeMessage = _welcomeMessage;
        Config::cfg.Save();
    }

    if (elText("Discord Invite Link", _discord,
        "Automatically print invite link in chat with !discord")) {
        Config::cfg.chat.discord = _discord;
        Config::cfg.Save();
    }

    if (elCheckbox("Flash Window on Message", Config::cfg.general.flashWindow,
        "The window will flash when a message is received, when not focused.")) {
        Config::cfg.Save();
    }

    if (elCheckbox("Text-to-Speech Chat", Config::cfg.chat.ttsEnabled,
        "Reads incoming visible chat messages aloud using Windows text-to-speech.")) {
        Config::cfg.Save();
    }

    if (elCheckbox("Disable !bonk", Config::cfg.chat.bonkEnabled,
        "Funny at first, but can quickly turn annoying.")) {
        Config::cfg.Save();
    }

    if (elCheckbox("Disable Guide Button", Config::cfg.input.disableGuideButton,
        "The guide button by default often brings up overlays in software, which can cause issues when hosting.")) {
        Config::cfg.Save();
    }

    if (elCheckbox("Disable Keyboard", Config::cfg.input.disableKeyboard,
        "Prevents users without gamepads playing with keyboard.")) {
        Config::cfg.Save();
    }

    if (elCheckbox("Auto Index Gamepads", Config::cfg.input.autoIndex,
        "XInput indices will be identified automatically. Beware, this may cause BSOD crashes for some users!")) {
        Config::cfg.Save();
    }

    if (elCheckbox("Parsec Logs", Config::cfg.general.parsecLogs,
        "When enabled this will print logs from Parsec in the Log window.")) {
        Config::cfg.Save();
    }

    if (elCheckbox("IP Address Bans", Config::cfg.general.ipBan,
        "When banning a user, their IP address will be banned from joining your room also.")) {
        Config::cfg.Save();
    }

    if (elCheckbox("WebSocket Server", Config::cfg.socket.enabled,
        "When hosting, Smash Soda will create a WebSocket server that sends data to clients. Required for the overlay!")) {
        Config::cfg.Save();
    }

    if (elNumber("WebSocket Port", _socketPort, 0, 65535,
        "The port the WebSocket server will run on.")) {
        Config::cfg.socket.port = _socketPort;
        Config::cfg.Save();

        if (_socketPort && WebSocket::instance.isRunning()) {
            WebSocket::instance.stopServer();
        }
        else {
            WebSocket::instance.createServer(Config::cfg.socket.port);
        }
    }

    if (elCheckbox("Block VPNs", Config::cfg.general.blockVPN,
        "It is advisable to only enable this if you are having issues with trolls, as some users use VPNs legitimately.")) {
        Config::cfg.Save();
    }

    if (elCheckbox("Developer Mode", Config::cfg.general.devMode,
        "Enables extra developer options for testing Smash Soda. Only for those who know what they are doing!")) {
        Config::cfg.Save();
    }

    EndTabBody();
}

void SettingsWidget::renderChatbot() {
    BeginTabBody("##settings-body-chatbot");

    ImGui::BeginGroup();
    ImGui::Indent(10);
    AppColors::pushButtonSolid();
    if (ImGui::Button("Refresh SFX List")) {
        Cache::cache.reloadSfxList();
    }
    ImGui::PopStyleColor(4);
    ImGui::Unindent(10);
    ImGui::EndGroup();

    if (ImForm::InputCheckbox("Host can't be bonked", _hostBonkProof,
        "You DARE bonk the host!?")) {
        Config::cfg.chat.hostBonkProof = _hostBonkProof;
        Config::cfg.Save();
    }

    EndTabBody();
}

void SettingsWidget::renderSfx() {
    static std::vector<SfxRow> rows;
    static bool loaded = false;
    static std::string statusMessage;
    static bool statusError = false;

    auto reloadRows = [&]() {
        statusError = !LoadSfxRows(rows, statusMessage);
        loaded = true;
    };

    if (!loaded) {
        reloadRows();
    }

    BeginTabBody("##settings-body-sfx");

    if (Widget::elBtnSecondary("Rescan SFX Folder")) {
        Cache::cache.reloadSfxList();
        reloadRows();
    }

    ImGui::SameLine();
    Theme* theme = ThemeController::getInstance().getActiveTheme();
    ImGui::PushFont(AppFonts::label);
    ImGui::PushStyleColor(ImGuiCol_Text, theme->formLabel);
    ImGui::Text("Loaded: %d", static_cast<int>(rows.size()));
    ImGui::PopStyleColor();
    ImGui::PopFont();

    if (!statusMessage.empty()) {
        ImGui::PushFont(AppFonts::label);
        ImGui::PushStyleColor(ImGuiCol_Text, statusError ? ImVec4(1.0f, 0.3f, 0.3f, 1.0f) : theme->formHelpText);
        ImGui::Text("%s", statusMessage.c_str());
        ImGui::PopStyleColor();
        ImGui::PopFont();
    }

    ImGui::Dummy(ImVec2(0, 8));

    const ImVec2 avail = ImGui::GetContentRegionAvail();
    const float tableBottomPadding = 10.0f;
    const float tableHeight = (avail.y > tableBottomPadding) ? (avail.y - tableBottomPadding) : 0.0f;
    const float tableRightPadding = 12.0f;
    const float tableWidth = (avail.x > tableRightPadding) ? (avail.x - tableRightPadding) : avail.x;
    const ImGuiTableFlags flags = ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersInnerV |
        ImGuiTableFlags_BordersOuter | ImGuiTableFlags_ScrollY | ImGuiTableFlags_SizingStretchProp;

    ImGui::PushStyleColor(ImGuiCol_TableHeaderBg, theme->tabsBackground);
    ImGui::PushStyleColor(ImGuiCol_Border, theme->panelBorder);
    ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(8, 4));

    if (ImGui::BeginTable("sfx-table", 2, flags, ImVec2(tableWidth, tableHeight))) {
        ImGui::TableSetupColumn("Sound", ImGuiTableColumnFlags_WidthStretch, 0.75f);
        ImGui::TableSetupColumn("Cooldown (sec)", ImGuiTableColumnFlags_WidthFixed, 140.0f);
        ImGui::TableHeadersRow();

        for (int i = 0; i < static_cast<int>(rows.size()); i++) {
            ImGui::TableNextRow();

            ImGui::TableSetColumnIndex(0);
            ImGui::PushFont(AppFonts::input);
            ImGui::PushStyleColor(ImGuiCol_Text, theme->panelText);
            ImGui::TextUnformatted(rows[i].tag.c_str());
            ImGui::PopStyleColor();
            ImGui::PopFont();

            ImGui::TableSetColumnIndex(1);
            std::string inputId = "##sfx-cooldown-" + std::to_string(i);
            ImGui::SetNextItemWidth(120.0f);
            if (ImGui::InputInt(inputId.c_str(), &rows[i].cooldown, 1, 5)) {
                if (rows[i].cooldown < 0) {
                    rows[i].cooldown = 0;
                }

                if (SaveSfxRows(rows)) {
                    Cache::cache.reloadSfxList();
                    statusMessage = "Saved.";
                    statusError = false;
                }
                else {
                    statusMessage = "Failed to save sfx.json.";
                    statusError = true;
                }
            }
        }

        ImGui::EndTable();
    }

    ImGui::PopStyleVar();
    ImGui::PopStyleColor(2);

    EndTabBody();
}

void SettingsWidget::renderPermissions() {
    BeginTabBody("##settings-body-permissions");

    Theme* theme = ThemeController::getInstance().getActiveTheme();
    ImGui::PushFont(AppFonts::title);
    ImGui::PushStyleColor(ImGuiCol_Text, theme->panelText);
    ImGui::Text("Regular Guest");
    ImGui::PopStyleColor();
    ImGui::PopFont();

    if (ImForm::InputCheckbox("Can use !sfx command", _guestSFX)) {
        Config::cfg.permissions.guest.useSFX = _guestSFX;
        Config::cfg.Save();
    }

    if (ImForm::InputCheckbox("Can use !bb command", _guestBB)) {
        Config::cfg.permissions.guest.useBB = _guestBB;
        Config::cfg.Save();
    }

    if (ImForm::InputCheckbox("Can change keyboard controls", _guestControls)) {
        Config::cfg.permissions.guest.changeControls = _guestControls;
        Config::cfg.Save();
    }

    ImGui::PushFont(AppFonts::title);
    ImGui::PushStyleColor(ImGuiCol_Text, theme->panelText);
    ImGui::Text("VIPs");
    ImGui::PopStyleColor();
    ImGui::PopFont();

    if (ImForm::InputCheckbox("Can use !sfx command", _vipSFX)) {
        Config::cfg.permissions.vip.useSFX = _vipSFX;
        Config::cfg.Save();
    }

    if (ImForm::InputCheckbox("Can use !bb command", _vipBB)) {
        Config::cfg.permissions.vip.useBB = _vipBB;
        Config::cfg.Save();
    }

    if (ImForm::InputCheckbox("Can change keyboard controls", _vipControls)) {
        Config::cfg.permissions.vip.changeControls = _vipControls;
        Config::cfg.Save();
    }

    ImGui::PushFont(AppFonts::title);
    ImGui::PushStyleColor(ImGuiCol_Text, theme->panelText);
    ImGui::Text("Moderators");
    ImGui::PopStyleColor();
    ImGui::PopFont();

    if (ImForm::InputCheckbox("Can use !sfx command", _modSFX)) {
        Config::cfg.permissions.moderator.useSFX = _modSFX;
        Config::cfg.Save();
    }

    if (ImForm::InputCheckbox("Can use !bb command", _modBB)) {
        Config::cfg.permissions.moderator.useBB = _modBB;
        Config::cfg.Save();
    }

    if (ImForm::InputCheckbox("Can change keyboard controls", _modControls)) {
        Config::cfg.permissions.moderator.changeControls = _modControls;
        Config::cfg.Save();
    }

    EndTabBody();
}

void SettingsWidget::renderHotkeys() {
    BeginTabBody("##settings-body-hotkeys");

    if (Config::cfg.mapHotkey) {
        Theme* theme = ThemeController::getInstance().getActiveTheme();
        ImGui::PushFont(AppFonts::title);
        ImGui::PushStyleColor(ImGuiCol_Text, theme->panelText);
        ImGui::TextWrapped("Press a key to map to the command: %s", Config::cfg.pendingHotkeyCommand.c_str());
        ImGui::PopStyleColor();
        ImGui::PopFont();
    }
    else if (_showHotkeyForm) {
        if (ImForm::InputText("CHAT COMMAND", _hotkeyCommand,
            "What is the command you would like to map to this key.")) {
            Config::cfg.pendingHotkeyCommand = _hotkeyCommand;
        }

        ImGui::BeginGroup();
        ImGui::Indent(10);
        AppColors::pushButtonSolid();
        if (ImGui::Button("Set Key") && Config::cfg.pendingHotkeyCommand != "") {
            Config::cfg.SetHotkey();
            _showHotkeyForm = false;
        }
        ImGui::PopStyleColor(4);
        ImGui::Unindent(10);
        ImGui::EndGroup();
    }
    else {
        ImGui::BeginGroup();
        ImGui::Indent(10);
        AppColors::pushButtonSolid();
        if (ImGui::Button("Add Hotkey")) {
            _showHotkeyForm = true;
        }
        ImGui::PopStyleColor(4);
        ImGui::Unindent(10);
        ImGui::EndGroup();

        ImGui::Dummy(ImVec2(0, 10));
        ImGui::Separator();
        ImGui::Dummy(ImVec2(0, 10));

        ImGui::BeginChild("hotkeylist");

        for (int i = 0; i < Config::cfg.hotkeys.keys.size(); i++) {
            if (IconButton::render(AppIcons::trash, AppColors::primary, ImVec2(30, 30))) {
                Config::cfg.RemoveHotkey(i);
            }

            ImGui::SameLine();
            ImGui::BeginGroup();
            ImGui::Indent(10);
            Theme* theme = ThemeController::getInstance().getActiveTheme();
            ImGui::PushFont(AppFonts::input);
            ImGui::PushStyleColor(ImGuiCol_Text, theme->formInputText);
            ImGui::Text("%s", Config::cfg.hotkeys.keys[i].command.c_str());
            ImGui::PopStyleColor();
            ImGui::PopFont();
            ImGui::PushFont(AppFonts::label);
            ImGui::PushStyleColor(ImGuiCol_Text, theme->formLabel);
            ImGui::Text("%s", ("CTRL + " + Config::cfg.hotkeys.keys[i].keyName).c_str());
            ImGui::PopStyleColor();
            ImGui::PopFont();
            ImGui::Unindent(10);
            ImGui::EndGroup();

            ImGui::Dummy(ImVec2(0, 5));
        }

        ImGui::EndChild();
    }

    EndTabBody();
}
