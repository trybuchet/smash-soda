#include "GuestListWidget.h"
#include "../core/GamepadClient.h"

GuestListWidget::GuestListWidget(Hosting& hosting)
    : _hosting(hosting), _guests(hosting.getGuests()), _guestHistory(_hosting.getGuestHistory())
{
}

bool GuestListWidget::render(bool& showWindow) {

    startWidget("Guests", showWindow, 782, 10, 400, 800, 350, 400);

    std::vector<Tab> tabs = {
        {"Online",  [&]() { renderOnlineGuests(); }},
        {"Bans",    [&]() { renderBannedGuests(); }},
        {"Mods",    [&]() { renderModdedGuests(); }},
        {"VIP",     [&]() { renderVIPGuests(); }},
        {"History", [&]() { renderHistoryGuests(); }}
    };
    startTabs(tabs);
    endTabs();
    endWidget();

    return true;
}

void GuestListWidget::guestItem(Guest& guest) {

    ImGui::PushStyleColor(ImGuiCol_Text, AppColors::panelText);
    ImGui::Text("%s", guest.name.c_str());
    ImGui::PopStyleColor();
    ImGui::SameLine();
    ImGui::PushStyleColor(ImGuiCol_Text, AppColors::formHelpText);
    ImGui::Text("#%d", guest.userID);

}

void GuestListWidget::renderOnlineGuests() {

    Theme* theme = ThemeController::getInstance().getActiveTheme();

    ImGui::Unindent(20);
    ImGui::BeginChild("filterbar", ImVec2(size.x, 81));
    ImGui::SetCursorPos(ImVec2(0, 0));
        ImDrawList* drawList = ImGui::GetWindowDrawList();
        pos = ImGui::GetCursorScreenPos();
        ImGui::Dummy(ImVec2(0, 10));
        drawList->AddRectFilled(pos, ImVec2(pos.x + size.x, pos.y + 84), ImGui::ColorConvertFloat4ToU32(ImVec4(0,0,0,0.1)), 8, ImDrawFlags_RoundCornersBottom);
        ImGui::Indent(20);
        elText("", _filterText, "Filter by name or ID");
        ImGui::Unindent(20);
    ImGui::EndChild();
    ImGui::SetCursorPos(ImVec2(0, 148));
    ImGui::Separator();
    ImGui::Indent(20);

    size = ImGui::GetContentRegionAvail();
    static size_t popupIndex;
    static string name;
    static uint32_t userID;
    static MyMetrics m;
    static ImVec2 cursor;

    // Guests
    static bool showKickPopup = false;
    static string kickPopupTitle = "";
    static bool showBanPopup = false;
    static string banPopupTitle = "";
    static string filterTextStr;
    static bool filterSuccess = false;

    ImGui::BeginChild("onlinelist");
    ImGui::Dummy(ImVec2(0, 20));
    isBodyFocused = ImGui::IsWindowFocused();

    for (size_t i = 0; i < _guests.size(); ++i) {
        name = _guests[i].name;
        userID = _guests[i].userID;

        bool allowMouseInput = _guests[i].allowMouseInput;
        bool allowKeyboardInput = _guests[i].allowKeyboardInput;

        m = _hosting.getMetrics(_guests[i].id);

        filterTextStr = _filterText;
        if (!filterTextStr.empty()) {
            filterSuccess = (Stringer::fuzzyDistance(_filterText, name) == 0);
            if (!filterSuccess) {
                filterSuccess = (Stringer::fuzzyDistance(_filterText, to_string(userID)) == 0);
            }
            if (!filterSuccess) {
                continue;
            }
        }

        cursor = ImGui::GetCursorPos();
        ImGui::BeginGroup();

        ImGui::Indent(10);
        guestItem(_guests[i]);

        ImGui::SameLine();
        if (m.congested == 2) {
            ImGui::PushStyleColor(ImGuiCol_Text, theme->negative);
        }
        else if (m.congested == 1) {
            ImGui::PushStyleColor(ImGuiCol_Text, theme->panelText);
        }
        else {
            ImGui::PushStyleColor(ImGuiCol_Text, theme->positive);
        }
        if (m.metrics.packetsSent < 1) {
            ImGui::Text("  N/A");
        } else {

            ImGui::Text("  %.0fms",
                m.metrics.networkLatency
                //m.metrics.bitrate,
                //m.metrics.queuedFrames,
                //m.metrics.decodeLatency,
                //m.metrics.encodeLatency,
                //m.metrics.slowRTs,
                //m.metrics.fastRTs
            );

        }
        ImGui::PopStyleColor();

        ImGui::Dummy(ImVec2(-8, 0));

        ImGui::SameLine();
        //ImGui::Image(AppIcons::userOn, ImVec2(16, 16), ImVec2(0, 0), ImVec2(1, 1), theme->panelText);

        if (allowMouseInput) {
            ImGui::SameLine();
            ImGui::Image(AppIcons::mouse, ImVec2(16, 16), ImVec2(0, 0), ImVec2(1, 1), theme->panelText);
        }

        if (allowKeyboardInput) {
            ImGui::SameLine();
            ImGui::Image(AppIcons::keymap, ImVec2(16, 16), ImVec2(0, 0), ImVec2(1, 1), theme->panelText);
        }

        if (Cache::cache.modList.isModded(userID) || _hosting.getHost().userID == userID) {
            ImGui::SameLine();
            ImGui::Image(AppIcons::hammerIcon, ImVec2(16, 16), ImVec2(0, 0), ImVec2(1, 1), theme->panelText);
        }
        ImGui::SameLine();
        if (Cache::cache.vipList.isVIP(userID)) {
            ImGui::SameLine();
            ImGui::Image(AppIcons::star, ImVec2(16, 16), ImVec2(0, 0), ImVec2(1, 1), theme->panelText);
        }
        ImGui::SameLine();
        // if (MetadataCache::isSpectating(userID)) {
        //     ImGui::SameLine();
        //     ImGui::Image(AppIcons::eye, ImVec2(16, 16), ImVec2(0, 0), ImVec2(1, 1), theme->panelText);
        // }
        ImGui::SameLine();
        if (Config::cfg.hotseat.enabled && Hotseat::instance.inSeat(userID)) {
            ImGui::SameLine();
            ImGui::Image(AppIcons::hotseat, ImVec2(16, 16), ImVec2(0, 0), ImVec2(1, 1), theme->panelText);
        }
        ImGui::Unindent(10);

        ImGui::Dummy(ImVec2(0.0f, 5.0f));
        ImGui::EndGroup();

        ImGui::SetCursorPos(cursor);

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.3f, 0.3f, 0.2f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.3f, 0.3f, 0.1f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.3f, 0.3f, 0.3f, 0.5f));

        ImGui::Button((string("##") + to_string(i + 1)).c_str(), ImVec2(size.x - 45, 48));

        ImGui::PopStyleColor(3);

        // Right click context menu for group
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(20, 10));
        //ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(5, 5));
        if (ImGui::BeginPopupContextItem()) {

            ImGui::PushStyleColor(ImGuiCol_Text, theme->textHighlighted);
            ImGui::Text("%s", name.c_str());
            ImGui::Dummy(ImVec2(0.0f, 10.0f));
            ImGui::PushStyleColor(ImGuiCol_Text, theme->panelText);

            if (ImGui::MenuItem("Allow keyboard input", nullptr, allowKeyboardInput)) {
                _hosting.setGuestInputPermissions(userID, !allowKeyboardInput, _guests[i].allowMouseInput);
            }

            if (ImGui::MenuItem("Allow mouse input", nullptr, allowMouseInput)) {
                _hosting.setGuestInputPermissions(userID, _guests[i].allowKeyboardInput, !allowMouseInput);
            }

            ImGui::Dummy(ImVec2(0.0f, 2.0f));

            if (Cache::cache.vipList.isVIP(userID)) {
                if (ImGui::Selectable("Revoke VIP Status")) {
                    Cache::cache.vipList.unVIP(userID);
                }
            }
            else {
                if (ImGui::Selectable("Make VIP")) {
                    Cache::cache.vipList.VIP(GuestData(name, userID));
                }
            }

            ImGui::Dummy(ImVec2(0.0f, 2.0f));

            if (ImGui::Selectable("Make spectator")) {
                _hosting.sendHostMessage((
                    string("!spectate ") + to_string(userID)
                    ).c_str(), true);
            }

            ImGui::Dummy(ImVec2(0.0f, 2.0f));

            if (ImGui::Selectable("Kick Guest")) {
                _hosting.sendHostMessage((
                    string("!kick ") + to_string(userID)
                    ).c_str(), true);
            }

            ImGui::Dummy(ImVec2(0.0f, 2.0f));

            if (Cache::cache.banList.isBanned(userID)) {
                if (ImGui::Selectable("Unban Guest")) {
                    Cache::cache.banList.unban(userID);
                }
            }
            else {
                if (ImGui::Selectable("Ban Guest")) {
                    Cache::cache.banList.ban(GuestData(name, userID));
                }
            }

            ImGui::Dummy(ImVec2(0.0f, 2.0f));

            if (Cache::cache.modList.isModded(userID)) {
                if (ImGui::Selectable("Revoke Moderator Role")) {
                    Cache::cache.modList.unmod(userID);
                }
            }
            else {
                if (ImGui::Selectable("Make Moderator")) {
                    Cache::cache.modList.mod(GuestData(name, userID));
                }
            }

            ImGui::Dummy(ImVec2(0.0f, 2.0f));

            if (ImGui::Selectable("Create Keyboard Profile")) {
                GamepadClient::instance.getKeyMap().createProfile(name, userID);
            }

            ImGui::EndPopup();

            ImGui::PopStyleColor();
            ImGui::PopStyleColor();
        }
        ImGui::PopStyleVar();

        if (ImGui::BeginDragDropSource()) {
            ImGui::SetDragDropPayload("Guest", &i, sizeof(int));
            ImGui::Text("%s", name.c_str());
            ImGui::Text("Drop into a Gamepad to assign.");
            ImGui::EndDragDropSource();
        }

    }

    ImGui::Dummy(ImVec2(0, 20));
    ImGui::EndChild();
}

void GuestListWidget::renderBannedGuests() {

    ImGui::Unindent(20);
    ImGui::BeginChild("filterbar", ImVec2(size.x, 81));
    ImGui::SetCursorPos(ImVec2(0, 0));
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    pos = ImGui::GetCursorScreenPos();
    ImGui::Dummy(ImVec2(0, 10));
    drawList->AddRectFilled(pos, ImVec2(pos.x + size.x, pos.y + 84), ImGui::ColorConvertFloat4ToU32(ImVec4(0, 0, 0, 0.1)), 8);
    ImGui::Indent(20);
    elText("", _filterText, "Filter by name or ID");
    ImGui::Unindent(20);
    ImGui::EndChild();
    ImGui::SetCursorPos(ImVec2(0, 148));
    ImGui::Separator();
    ImGui::Indent(20);

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(1, 1));
    ImGui::BeginChild("bannedlist");
    isBodyFocused = ImGui::IsWindowFocused();

    static bool showEditPopup = false;
    static size_t popupEditIndex;
    static string reason;
    static bool showUnbanPopup = false;
    static string popupTitle = "";
    static size_t popupIndex;
    static string name;
    static uint32_t userID;
    static vector<GuestData>& _bannedGuests = Cache::cache.banList.getGuests();
    static string filterTextStr;
    static bool filterSuccess = false;

    ImGui::Dummy(ImVec2(0.0f, 20.0f));

    for (size_t i = 0; i < _bannedGuests.size(); ++i) {
        name = _bannedGuests[i].name;
        userID = _bannedGuests[i].userID;
        reason = _bannedGuests[i].reason;

        filterTextStr = _filterText;
        if (!filterTextStr.empty()) {
            filterSuccess = (Stringer::fuzzyDistance(_filterText, name) == 0);
            if (!filterSuccess) {
                filterSuccess = (Stringer::fuzzyDistance(_filterText, to_string(userID)) == 0);
            }
            if (!filterSuccess && reason.size() > 0) {
                filterSuccess = (Stringer::fuzzyDistance(_filterText, reason) == 0);
            }

            if (!filterSuccess) {
                continue;
            }
        }

        if (_filterNoReason && reason.size() > 0) continue;

        IconButton::render(AppIcons::userOff, AppColors::buttonPrimary, ImVec2(30, 30));
        if (ImGui::IsItemActive()) {
            popupTitle = string("Unban##Popup");
            showUnbanPopup = true;
            popupIndex = i;
            ImGui::OpenPopup(popupTitle.c_str());
        }
        TitleTooltipWidget::render(
            "Blocked user",
            (string("Press to unban ") + name + "").c_str()
        );

        if (i == popupIndex)
        {
            if (ConfirmPopupWidget::render(
                popupTitle.c_str(),
                showUnbanPopup,
                ("Unban\n#" + to_string(userID) + "\n"+ name).c_str()
            ))
            {
                Cache::cache.banList.unban(userID);
            }
        }

        ImGui::SameLine();
        IconButton::render(AppIcons::editReason, AppColors::buttonSecondary, ImVec2(30, 30));
        if (ImGui::IsItemActive())
        {
            showEditPopup = true;
            popupEditIndex = i;
            ImGui::OpenPopup(string("Add reason").c_str());
        }
        TitleTooltipWidget::render("Add reason", string("Press to add reason").c_str());
        if (i == popupEditIndex)
        {
            if (PopupWidgetEdit::render(string("Add reason").c_str(), showEditPopup, reason))
            {
                _bannedGuests[i].reason = reason;
                Cache::cache.banList.SaveToFile();
            }
        }

        ImGui::SameLine();
        
        ImGui::BeginGroup();
            
            ImGui::PushStyleColor(ImGuiCol_Text, AppColors::panelText);
            ImGui::Text("%s", _bannedGuests[i].name.c_str());
            ImGui::PopStyleColor();
            ImGui::SameLine();
            ImGui::PushStyleColor(ImGuiCol_Text, AppColors::formHelpText);
            ImGui::Text("#%d", _bannedGuests[i].userID);
            ImGui::Text(_bannedGuests[i].reason.c_str());
            ImGui::PopStyleColor();

        ImGui::EndGroup();
    }

    ImGui::EndChild();
    ImGui::PopStyleVar();
}


void GuestListWidget::renderHistoryGuests() {

    ImGui::Unindent(20);
    ImGui::BeginChild("filterbar", ImVec2(size.x, 81));
    ImGui::SetCursorPos(ImVec2(0, 0));
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    pos = ImGui::GetCursorScreenPos();
    ImGui::Dummy(ImVec2(0, 10));
    drawList->AddRectFilled(pos, ImVec2(pos.x + size.x, pos.y + 84), ImGui::ColorConvertFloat4ToU32(ImVec4(0, 0, 0, 0.1)), 8, ImDrawFlags_RoundCornersBottom);
    ImGui::Indent(20);
    elText("", _filterText, "Filter by name or ID");
    ImGui::Unindent(20);
    ImGui::EndChild();
    ImGui::SetCursorPos(ImVec2(0, 148));
    ImGui::Separator();
    ImGui::Indent(20);

    ImGui::BeginChild("historylist");
    static bool showBanPopup = false;
    static string popupTitle = "";
    static size_t popupIndex;
    static string name;
    static uint32_t userID;
    static string filterTextStr;
    static bool filterSuccess = false;
    isBodyFocused = ImGui::IsWindowFocused();

    ImGui::Dummy(ImVec2(0.0f, 20.0f));

    for (size_t i = 0; i < _guestHistory.size(); ++i)
    {
        name = _guestHistory[i].name;
        userID = _guestHistory[i].userID;

        filterTextStr = _filterText;
        if (!filterTextStr.empty())
        {
            filterSuccess = (Stringer::fuzzyDistance(_filterText, name) == 0);
            if (!filterSuccess)
            {
                filterSuccess = (Stringer::fuzzyDistance(_filterText, to_string(userID)) == 0);
            }

            if (!filterSuccess)
            {
                continue;
            }
        }

        IconButton::render(AppIcons::block, AppColors::buttonPrimary, ImVec2(30, 30));
        if(ImGui::IsItemActive())
        {
            showBanPopup = true;
            popupTitle = string("Ban##Popup");
            popupIndex = i;
            ImGui::OpenPopup(popupTitle.c_str());
        }

        TitleTooltipWidget::render("Allowed user", (string("Press to ban ") + name).c_str());

        if (i == popupIndex)
        {
            if (ConfirmPopupWidget::render(
                popupTitle.c_str(),
                showBanPopup,
                ("Ban\n#" + to_string(userID) + "\n" + name).c_str()
            ))
            {
                _hosting.sendHostMessage((
                    string("!ban ") + to_string(userID)
                ).c_str(), true);
            }
        }

        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(1, 1));
        ImGui::SameLine();

        ImGui::BeginGroup();
        ImGui::PushStyleColor(ImGuiCol_Text, AppColors::panelText);
        ImGui::Text("%s", name.c_str());
        ImGui::PopStyleColor();
        ImGui::PushStyleColor(ImGuiCol_Text, AppColors::formHelpText);
        ImGui::Text("#%d", userID);
        ImGui::EndGroup();
        ImGui::PopStyleVar();
    }
    ImGui::EndChild();
}

void GuestListWidget::renderModdedGuests() {

    ImGui::Unindent(20);
    ImGui::BeginChild("filterbar", ImVec2(size.x, 81));
    ImGui::SetCursorPos(ImVec2(0, 0));
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    pos = ImGui::GetCursorScreenPos();
    ImGui::Dummy(ImVec2(0, 10));
    drawList->AddRectFilled(pos, ImVec2(pos.x + size.x, pos.y + 84), ImGui::ColorConvertFloat4ToU32(ImVec4(0, 0, 0, 0.1)), 8, ImDrawFlags_RoundCornersBottom);
    ImGui::Indent(20);
    elText("", _filterText, "Filter by name or ID");
    ImGui::Unindent(20);
    ImGui::EndChild();
    ImGui::SetCursorPos(ImVec2(0, 148));
    ImGui::Separator();
    ImGui::Indent(20);

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(1, 1));
    ImGui::BeginChild("moddedlist");
    isBodyFocused = ImGui::IsWindowFocused();

    static bool showEditPopup = false;
    static size_t popupEditIndex;
    static bool showUnmodPopup = false;
    static string popupTitle = "";
    static size_t popupIndex;
    static string name;
    static string reason;
    static uint32_t userID;
    static vector<GuestData>& _moddedGuests = Cache::cache.modList.getGuests();
    static string filterTextStr;
    static bool filterSuccess = false;

    ImGui::Dummy(ImVec2(0.0f, 20.0f));

    for (size_t i = 0; i < _moddedGuests.size(); ++i)
    {
        name = _moddedGuests[i].name;
        userID = _moddedGuests[i].userID;
        reason = _moddedGuests[i].reason;

        filterTextStr = _filterText;
        if (!filterTextStr.empty())
        {
            filterSuccess = (Stringer::fuzzyDistance(_filterText, name) == 0);
            if (!filterSuccess)
            {
                filterSuccess = (Stringer::fuzzyDistance(_filterText, to_string(userID)) == 0);
            }
            if (!filterSuccess && reason.size() > 0)
            {
                filterSuccess = (Stringer::fuzzyDistance(_filterText, reason) == 0);
            }

            if (!filterSuccess)
            {
                continue;
            }
        }

        if (_filterNoReason && reason.size() > 0) continue;

        IconButton::render(AppIcons::userOff, AppColors::buttonPrimary, ImVec2(30, 30));
        if (ImGui::IsItemActive())
        {
            popupTitle = string("Unmod##Popup");
            showUnmodPopup = true;
            popupIndex = i;
            ImGui::OpenPopup(popupTitle.c_str());
        }
        TitleTooltipWidget::render(
            "Moderator",
            (string("Press to unmod ") + name + "").c_str()
        );

        if (i == popupIndex)
        {
            if (ConfirmPopupWidget::render(
                popupTitle.c_str(),
                showUnmodPopup,
                ("Unmod\n#" + to_string(userID) + "\n" + name).c_str()
            ))
            {
                Cache::cache.modList.unmod(userID);
            }
        }

        ImGui::SameLine();

        ImGui::BeginGroup();
        ImGui::PushStyleColor(ImGuiCol_Text, AppColors::panelText);
        ImGui::Text("%s", name.c_str());
        ImGui::PopStyleColor();
        ImGui::PushStyleColor(ImGuiCol_Text, AppColors::formHelpText);
        ImGui::Text("#%d", userID);
        ImGui::EndGroup();
    }

    ImGui::EndChild();
    ImGui::PopStyleVar();

    //AppFonts::pushInput();
    //AppColors::pushTitle();
}

void GuestListWidget::renderVIPGuests() {

    ImGui::Unindent(20);
    ImGui::BeginChild("filterbar", ImVec2(size.x, 81));
    ImGui::SetCursorPos(ImVec2(0, 0));
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    pos = ImGui::GetCursorScreenPos();
    ImGui::Dummy(ImVec2(0, 10));
    drawList->AddRectFilled(pos, ImVec2(pos.x + size.x, pos.y + 84), ImGui::ColorConvertFloat4ToU32(ImVec4(0, 0, 0, 0.1)), 8, ImDrawFlags_RoundCornersBottom);
    ImGui::Indent(20);
    elText("", _filterText, "Filter by name or ID");
    ImGui::Unindent(20);
    ImGui::EndChild();
    ImGui::SetCursorPos(ImVec2(0, 148));
    ImGui::Separator();
    ImGui::Indent(20);

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(1, 1));
    ImGui::BeginChild("viplist");
    isBodyFocused = ImGui::IsWindowFocused();

    static bool showEditPopup = false;
    static size_t popupEditIndex;
    static bool showUnmodPopup = false;
    static string popupTitle = "";
    static size_t popupIndex;
    static string name;
    static string reason;
    static uint32_t userID;
    static vector<GuestData>& _moddedGuests = Cache::cache.vipList.getGuests();
    static string filterTextStr;
    static bool filterSuccess = false;

    ImGui::Dummy(ImVec2(0.0f, 20.0f));

    for (size_t i = 0; i < _moddedGuests.size(); ++i)
    {
        name = _moddedGuests[i].name;
        userID = _moddedGuests[i].userID;
        //reason = _moddedGuests[i].reason;

        filterTextStr = _filterText;
        if (!filterTextStr.empty())
        {
            filterSuccess = (Stringer::fuzzyDistance(_filterText, name) == 0);
            if (!filterSuccess)
            {
                filterSuccess = (Stringer::fuzzyDistance(_filterText, to_string(userID)) == 0);
            }
            if (!filterSuccess && reason.size() > 0)
            {
                filterSuccess = (Stringer::fuzzyDistance(_filterText, reason) == 0);
            }

            if (!filterSuccess)
            {
                continue;
            }
        }

        if (_filterNoReason && reason.size() > 0) continue;

        IconButton::render(AppIcons::userOff, AppColors::buttonPrimary, ImVec2(30, 30));
        if (ImGui::IsItemActive())
        {
            popupTitle = string("Unvip##Popup");
            showUnmodPopup = true;
            popupIndex = i;
            ImGui::OpenPopup(popupTitle.c_str());
        }
        TitleTooltipWidget::render(
            "VIP",
            (string("Press to unvip ") + name + "").c_str()
        );

        if (i == popupIndex)
        {
            if (ConfirmPopupWidget::render(
                popupTitle.c_str(),
                showUnmodPopup,
                ("Unvip\n#" + to_string(userID) + "\n" + name).c_str()
            ))
            {
                Cache::cache.vipList.unVIP(userID);
            }
        }

        ImGui::SameLine();

        ImGui::BeginGroup();
        ImGui::PushStyleColor(ImGuiCol_Text, AppColors::panelText);
        ImGui::Text("%s", name.c_str());
        ImGui::PopStyleColor();
        ImGui::PushStyleColor(ImGuiCol_Text, AppColors::formHelpText);
        ImGui::Text("#%d", userID);
        ImGui::EndGroup();
    }

    ImGui::EndChild();
    ImGui::PopStyleVar();

    //AppFonts::pushInput();
    //AppColors::pushTitle();
}


