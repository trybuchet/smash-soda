#include "GamepadsWidget.h"
#include "../core/GamepadClient.h"
#include "../services/ThemeController.h"
#include "../globals/AppFonts.h"

GamepadsWidget::GamepadsWidget(Hosting& hosting)
    : _hosting(hosting), _gamepads(GamepadClient::instance.gamepads)
{
}

bool GamepadsWidget::render(bool& showWindow) {

    static int
    xboxCount = (int)Config::cfg.input.xboxPuppetCount,
    lastXboxCount = (int)Config::cfg.input.xboxPuppetCount,
    ds4Count = (int)Config::cfg.input.ds4PuppetCount,
    lastDs4Count = (int)Config::cfg.input.ds4PuppetCount;

    startWidget("Virtual Gamepads", showWindow, 62, 278, 350, 600, 350, 400);

    startHeader("gamepadHeader", 50);

    Theme* theme = ThemeController::getInstance().getActiveTheme();

    if (!_hosting.getGamepadClient().isSlave) {

        static bool resetting = false;
        if (IconButton::render(AppIcons::refresh, AppColors::navbarIconActive, ImVec2(24, 24))) {
            if (!resetting) {
                resetting = true;
                _hosting.getGamepadClient().resetAll(resetting);
            }
        }
        TitleTooltipWidget::render("Reset gamepad engine", "If all else fails, try this button.\nPress in dire situations.");

        ImGui::SameLine();

        if (IconButton::render(AppIcons::padOff, AppColors::buttonSecondary, ImVec2(24, 24))) {
            _hosting.getGamepadClient().disconnectAllGamepads();
        }
        TitleTooltipWidget::render("Disconnect all gamepads", "Disconnects all of the gamepads currently enabled.");

        ImGui::SameLine();

        if (IconButton::render(AppIcons::sort, AppColors::buttonSecondary, ImVec2(24, 24))) {
            _hosting.getGamepadClient().sortGamepads();
        }
        TitleTooltipWidget::render("Sort gamepads", "Re-sort all gamepads by index.");

        ImGui::SameLine();

        if (ToggleIconButtonWidget::render(
            AppIcons::lock, AppIcons::unlock, _hosting.getGamepadClient().lock,
            AppColors::positive, AppColors::negative, ImVec2(24, 24)
        )) {
            _hosting.toggleGamepadLock();
        }
        if (_hosting.getGamepadClient().lock)   TitleTooltipWidget::render("Unlock guest inputs", "Guests will be able to control gamepads again.");
        else                            TitleTooltipWidget::render("Lock guest inputs", "Guest inputs will be locked out of gamepads.");

        ImGui::SameLine();

        if (ToggleIconButtonWidget::render(
            AppIcons::buttonLock, AppIcons::buttonLock, _hosting.getGamepadClient().lockButtons,
            AppColors::positive, AppColors::negative, ImVec2(24, 24)
        )) {
            _hosting.toggleGamepadLockButtons();
        }
        if (_hosting.getGamepadClient().lockButtons)
            TitleTooltipWidget::render("Unlock buttons", "Make sure you set it up");
        else
            TitleTooltipWidget::render("Lock buttons", "Make sure you set it up");

    } else {
        ImGui::Dummy(ImVec2(0, 10));
        ImGui::PushFont(AppFonts::label);
        ImGui::PushStyleColor(ImGuiCol_Text, theme->formLabel);
        ImGui::SetNextItemWidth(size.x - 20);
        ImGui::TextWrapped("You will not be able to use the gamepads widget whilst Smash Soda is in hotseat or tournament mode.");
        ImGui::PopStyleColor();
        ImGui::PopFont();
    }

    endHeader();

    size = ImGui::GetContentRegionAvail();
    pos = ImGui::GetCursorScreenPos();
    startBody(true);

    static vector<AnimatedGamepadWidget*> agws;

    static ImVec2 cursor;
    static bool isWindowLocked = false;
    static bool isConnectionButtonPressed = false;
    static bool refreshGamepads = false;
    static ImVec2 dummySize = ImVec2(0.0f, 5.0f);

    for (size_t i = 0; i < _gamepads.size(); ++i) {

        while (agws.size() <= i) {
            agws.push_back(new AnimatedGamepadWidget(true));
        }

        ImGui::PushID(static_cast<int>(i));

        AnimatedGamepadWidget* agw = agws[i];
        AGamepad* gi = _gamepads[i];

        int height = (gi->isConnected() ? 120 : 74);

        std::string child = "##Gamepad " + to_string(i);
        size = ImGui::GetContentRegionAvail();
        //ImGui::BeginChild(child.c_str(), ImVec2(size.x - 20, height));
        ImGui::BeginGroup();
            isFocused = ImGui::IsWindowFocused();
            pos = ImGui::GetCursorScreenPos();
            size = ImGui::GetContentRegionAvail();
            ImDrawList* drawList = ImGui::GetWindowDrawList();
            drawList->AddRectFilled(pos, ImVec2(pos.x + size.x - 20, pos.y + height), ImGui::ColorConvertFloat4ToU32(ImVec4(0, 0, 0, 0.1)), 8, ImDrawFlags_RoundCornersAll);

            ImGui::Dummy(ImVec2(0, 5));
            ImGui::Indent(10);

            renderPadInputTypeIcon(gi, i, refreshGamepads);

            ImGui::SameLine();

            ImGui::Indent(55);

            if (gi->isConnected()) {
                
                ImGui::BeginGroup();

                    ImGui::BeginGroup();

                        // Get the current cursor position (button's position)
                        ImVec2 buttonPos = ImGui::GetCursorScreenPos();
                        ImVec2 buttonSize = ImVec2((size.x - 100), 30.0f);

                        // Adjust text position to be centered above the button
                        ImVec2 textPos = ImVec2(buttonPos.x + 10, buttonPos.y + 5);

                        static std::string name, id;
                        if (gi->owner.guest.isValid()) {
                            id = "(# " + std::to_string(gi->owner.guest.userID) + ")";
                            name = gi->owner.guest.name;

                            // Draw floating text
                            ImGui::SetCursorScreenPos(textPos);
                            ImGui::PushStyleColor(ImGuiCol_Text, AppColors::panelText);
                            ImGui::Text("%s", name.c_str());
                            ImGui::PopStyleColor();

                            ImGui::SameLine();
                            ImGui::PushStyleColor(ImGuiCol_Text, AppColors::formHelpText);
                            ImGui::Text("%s", id.c_str());
                            ImGui::PopStyleColor();
                        }
                        else if (_hosting.getGamepadClient().isPuppetMaster && gi->isPuppet) {
                            id = "(# " + std::to_string(_hosting.getHost().userID) + ")";
                            name = _hosting.getHost().name;

                            ImGui::SetCursorScreenPos(textPos);
                            ImGui::PushStyleColor(ImGuiCol_Text, AppColors::panelText);
                            ImGui::Text("%s", name.c_str());
                            ImGui::PopStyleColor();

                            ImGui::SameLine();
                            ImGui::PushStyleColor(ImGuiCol_Text, AppColors::formHelpText);
                            ImGui::Text("%s", id.c_str());
                            ImGui::PopStyleColor();

                        } else {
                            ImGui::SetCursorScreenPos(textPos);
                            ImGui::PushStyleColor(ImGuiCol_Text, AppColors::formHelpText);
                            ImGui::Text(" ");
                            ImGui::PopStyleColor();
                        }

                        // Reset cursor to button position before drawing it
                        ImGui::SetCursorScreenPos(buttonPos);
                        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0.1));
                        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0, 0, 0, 0.2));
                        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0.3));
                        ImGui::Button((std::string("##gamepad button") + std::to_string(i + 1)).c_str(), buttonSize);

                            if (ImGui::BeginDragDropSource()) {
                                if (!_hosting.getGamepadClient().isSlave) {
                                    ImGui::SetDragDropPayload("Gamepad", &i, sizeof(int));
                            
                                    AppFonts::pushInput();
                                    AppColors::pushPrimary();
                                    ImGui::Text("%s", (gi->owner.guest.isValid() ? gi->owner.guest.name.c_str() : "Empty gamepad"));
                                    AppColors::pop();
                                    AppFonts::pop();
                            
                                    ImGui::PushFont(AppFonts::label);
                                    ImGui::PushStyleColor(ImGuiCol_Text, theme->formLabel);
                                    ImGui::Text("Drop into another Gamepad to swap.");
                                    ImGui::PopStyleColor();
                                    ImGui::PopFont();
                                }
                            
                                ImGui::EndDragDropSource();
                            }

                            if (ImGui::BeginDragDropTarget()) {
                                if (!_hosting.getGamepadClient().isSlave) {
                                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Guest")) {
                                            if (payload->DataSize == sizeof(int)) {
                                                int guestIndex = *(const int*)payload->Data;
                                                if (guestIndex >= 0 && guestIndex < _hosting.getGuests().size()) {
                                                    Guest& guest = _hosting.getGuests()[guestIndex];
                                                    if (!Config::cfg.hotseat.enabled || Hotseat::instance.checkUser(guest.userID, guest.name)) {

                                                        if (Config::cfg.hotseat.enabled) {

                                                            if (gi->owner.guest.isValid()) {
                                                                Hotseat::instance.pauseUser(gi->owner.guest.userID);
                                                            }

                                                            Hotseat::instance.seatUser(guest.userID, guest.name);
                                                        }

                                                        gi->owner.guest.copy(_hosting.getGuests()[guestIndex]);
                                                    }
                                                }
                                            }
                                        } 
                                        else if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Gamepad")) {
                                            if (payload->DataSize == sizeof(int) && !Config::cfg.hotseat.enabled) {
                                            int sourceIndex = *(const int*)payload->Data;
                                            if (sourceIndex >= 0 && sourceIndex < _gamepads.size()) {
                                                static GuestDevice backupOwner;
                                                backupOwner.copy(_gamepads[i]->owner);

                                                Guest& guest = _gamepads[sourceIndex]->owner.guest;

                                                _gamepads[i]->copyOwner(_gamepads[sourceIndex]);
                                                _gamepads[sourceIndex]->owner.copy(backupOwner);
                                                _gamepads[i]->clearState();
                                                _gamepads[sourceIndex]->clearState();

                                            }
                                        }
                                    }
                                }

                                ImGui::EndDragDropTarget();
                            }
                        
                    ImGui::EndGroup();

                    ImGui::BeginGroup();

                        if (IconButton::render(AppIcons::back, AppColors::buttonSecondary, ImVec2(24, 24))) {
                            if (!_hosting.getGamepadClient().isSlave) {
                                gi->clearOwner();
                            }
                        }
                        TitleTooltipWidget::render("Strip gamepad", "Unlink current user from this gamepad.");

                        ImGui::SameLine();

                        if (ToggleIconButtonWidget::render(AppIcons::padOff, AppIcons::padOn, gi->isConnected(), ImVec2(24, 24))) {
                            if (!_hosting.getGamepadClient().isSlave) {
                                if (gi->isConnected()) {
                                    gi->disconnect();
                                }
                                else {
                                    gi->connect();
                                }

                                isConnectionButtonPressed = true;
                            }
                        }
                        if (gi->isConnected()) TitleTooltipWidget::render("Connected gamepad", "Press to \"physically\" disconnect\nthis gamepad (at O.S. level).");
                        else                  TitleTooltipWidget::render("Disconnected gamepad", "Press to \"physically\" connect\nthis gamepad (at O.S. level).");

                        ImGui::SameLine();

                        if (gi->isLocked()) {
                            if (IconButton::render(AppIcons::lock, AppColors::negative, ImVec2(24, 24))) {
                                if (!_hosting.getGamepadClient().isSlave) {
                                    gi->toggleLocked();
                                }
                            }
                            TitleTooltipWidget::render("Locked gamepad", "Unlock this specific gamepad, allowing inputs and picking.");
                        }
                        else {
                            if (IconButton::render(AppIcons::unlock, AppColors::positive, ImVec2(24, 24))) {
                                if (!_hosting.getGamepadClient().isSlave) {
                                    gi->toggleLocked();
                                }
                            }
                            TitleTooltipWidget::render("Unlocked gamepad", "Lock this specific gamepad, preventing inputs or picking.");
                        }

                        ImGui::SameLine();

                        if (gi->isLockedButtons()) {
                            if (IconButton::render(AppIcons::buttonLock, AppColors::negative, ImVec2(24, 24))) {
                                if (!_hosting.getGamepadClient().isSlave) {
                                    gi->toggleLockedButtons();
                                }
                            }
                            TitleTooltipWidget::render("Unlock buttons", "Make sure you set it up");
                        }
                        else {
                            if (IconButton::render(AppIcons::buttonLock, AppColors::positive, ImVec2(24, 24))) {
                                if (!_hosting.getGamepadClient().isSlave) {
                                    gi->toggleLockedButtons();
                                }
                            }
                            TitleTooltipWidget::render("Lock buttons", "Make sure you set it up");
                        }
                    ImGui::EndGroup();        

                ImGui::EndGroup();

            } else {

                ImGui::BeginGroup();
                ImGui::Dummy(ImVec2(0, 5));
                if (elBtn("Connect Gamepad")) {
                    gi->connect();
                    isConnectionButtonPressed = true;
                }
                ImGui::EndGroup();

            }

            ImGui::Unindent(55);
            ImGui::Indent(8);
            ImGui::BeginGroup();
            if (gi->isConnected()) {
                agw->render(gi->getState().Gamepad, 30);
            }
            ImGui::EndGroup();
            ImGui::Unindent(5);

        ImGui::EndGroup();
        ImGui::Dummy(ImVec2(0, 10));

        ImGui::PopID();
    }

    ImGui::Dummy(ImVec2(0, 20));

    endBody();

    startFooter();
        ImGui::Dummy(ImVec2(10.0, .05f));
        if (!_hosting.getGamepadClient().isSlave) {

            ImGui::Image(AppIcons::xinput, ImVec2(24, 24), ImVec2(0, 0), ImVec2(1, 1), AppColors::buttonSecondary);
            ImGui::SameLine();
            if (IntRangeWidget::render("xboxCounter", xboxCount, 0, 32)) {
                TitleTooltipWidget::render("XBox Puppet Counter", "Set the amount of XBox controllers.\n\n* Warning: disconnect all gamepads before changing this.");
            }

            ImGui::SameLine();

            // Get size of window
            float _size = ImGui::GetContentRegionAvail().x;
            ImGui::SameLine();
            ImGui::SetCursorPosX(_size);

            ImGui::Image(AppIcons::dinput, ImVec2(24, 24), ImVec2(0, 0), ImVec2(1, 1), AppColors::buttonSecondary);
            ImGui::SameLine();
            if (IntRangeWidget::render("ds4Counter", ds4Count, 0, 32)) {
                TitleTooltipWidget::render("Dualshock Puppet Counter", "Set the amount of DS4 controllers.\n\n* Warning: disconnect all gamepads before changing this.");
            }

            ImGui::Dummy(ImVec2(0, 15));

        }

    endFooter();

    endWidget();

    static Stopwatch resizeGamepadsDebouncer = Stopwatch(500);

    if (refreshGamepads || (Config::cfg.input.autoIndex && isConnectionButtonPressed)) {
        refreshGamepads = false;
        isConnectionButtonPressed = false;
        for (size_t i = 0; i < _gamepads.size(); ++i) {
            _gamepads[i]->refreshIndex();
        }
    }

    if (lastXboxCount != xboxCount || lastDs4Count != ds4Count) {
        if (xboxCount > 32) xboxCount = 32;
        if (ds4Count > 32) ds4Count = 32;
        lastXboxCount = xboxCount;
        lastDs4Count = ds4Count;
        Config::cfg.input.xboxPuppetCount = xboxCount;
        Config::cfg.input.ds4PuppetCount = ds4Count;
        _hosting.getGamepadClient().resize(xboxCount, ds4Count);
        resizeGamepadsDebouncer.start();
        resizeGamepadsDebouncer.reset(500);
    }
    if (resizeGamepadsDebouncer.isRunning() && resizeGamepadsDebouncer.isFinished()) {
        resizeGamepadsDebouncer.stop();
        Config::cfg.Save();
    }

    return true;
}

//bool GamepadsWidget::render(bool& showWindow) {

    Theme* theme = ThemeController::getInstance().getActiveTheme();
//
//    for (size_t i = 0; i < _gamepads.size(); ++i)
//    {
//        while (agws.size() <= i) {
//            agws.push_back(new AnimatedGamepadWidget(true));
//        }
//        AnimatedGamepadWidget* agw = agws[i];
//        //AnimatedGamepadWidget agw;
//        AGamepad* gi = _gamepads[i];
//        static uint32_t userID;
//        userID = gi->owner.guest.userID;
//
//        ImGui::BeginChild(
//            (string("##Gamepad " ) + to_string(i)).c_str(),
//            ImVec2(size.x, 90)
//        );
//
//        cursor = ImGui::GetCursorPos();
//        static const ImVec2 ICON_SIZE = ImVec2(45, 45);
//        
//        static int xboxIndex = 0, padIndex = 0;
//        xboxIndex = (int)gi->getIndex();
//        padIndex = xboxIndex + 1;
//        static bool isIndexSuccess = false;
//        isIndexSuccess = gi->isConnected() && padIndex > 0 && padIndex <= 4;
//
//        ImGui::BeginGroup();
//        {
//            renderPadInputTypeIcon(gi, i, refreshGamepads);
//        }
//        ImGui::EndGroup();
//
//        ImGui::SameLine();
//        
//        static float gamepadLabelWidth;
//        gamepadLabelWidth = size.x - 75.0f;
//        
//        ImGui::BeginChild(
//            (string("##name ") + to_string(i)).c_str(),
//            ImVec2(gamepadLabelWidth, 80.0f)
//        );
//        cursor = ImGui::GetCursorPos();
//
//        //ImGui::Dummy(ImVec2(0,8));
//
//
//        static string name, id;
//        if (gi->owner.guest.isValid())
//        {
//            id = string() + "(# " + to_string(gi->owner.guest.userID) + ")\t";
//            name = gi->owner.guest.name;
//        }
//        else if (_hosting.getGamepadClient().isPuppetMaster && gi->isPuppet)
//        {
//            id = string() + "(# " + to_string(_hosting.getHost().userID) + ")\t";
//            name = _hosting.getHost().name;
//        }
//        else {
//            id = "    ";
//            name = "    ";
//        }
//
//        ImGui::Indent(5.0f);
//        ImGui::Dummy(ImVec2(0, 3.0f));
//
//        ImGui::PushFont(AppFonts::label);
//        //ImGui::TextWrapped(id.c_str());
//        ImGui::Text(id.c_str());
//        AppStyle::pop();
//
//        AppFonts::pushInput();
//        AppColors::pushPrimary();
//        ImGui::SetNextItemWidth(gamepadLabelWidth);
//        ImGui::Text(name.c_str());
//        AppColors::pop();
//        AppFonts::pop();
//
//        ImGui::Unindent();
//
//        static ImVec2 backupCursor;
//        backupCursor = ImGui::GetCursorPos();
//
//        ImGui::SetCursorPos(cursor);
//        if (gi->isConnected()) {
//            ImGui::Button(
//                (string("##gamepad button") + to_string(i + 1)).c_str(),
//                //ImVec2(gamepadLabelWidth, 50.0f)
//                ImVec2(gamepadLabelWidth, 35.0f)
//            );
//
//            if (ImGui::BeginDragDropSource())
//            {
//                if (!_hosting.getGamepadClient().isSlave) {
//                    ImGui::SetDragDropPayload("Gamepad", &i, sizeof(int));
//
//                    AppFonts::pushInput();
//                    AppColors::pushPrimary();
//                    ImGui::Text("%s", (gi->owner.guest.isValid() ? gi->owner.guest.name.c_str() : "Empty gamepad"));
//                    AppColors::pop();
//                    AppFonts::pop();
//
//                    ImGui::PushFont(AppFonts::label);
//                    ImGui::Text("Drop into another Gamepad to swap.");
//                    AppStyle::pop();
//                }
//
//                ImGui::EndDragDropSource();
//            }
//            if (ImGui::BeginDragDropTarget())
//            {
//                if (!_hosting.getGamepadClient().isSlave) {
//                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Guest"))
//                    {
//                        if (payload->DataSize == sizeof(int))
//                        {
//                            int guestIndex = *(const int*)payload->Data;
//                            if (guestIndex >= 0 && guestIndex < _hosting.getGuests().size()) {
//                                Guest& guest = _hosting.getGuests()[guestIndex];
//                                if (!Config::cfg.hotseat.enabled || Hotseat::instance.checkUser(guest.userID, guest.name)) {
//
//                                    if (Config::cfg.hotseat.enabled) {
//
//                                        if (gi->owner.guest.isValid()) {
//                                            Hotseat::instance.pauseUser(gi->owner.guest.userID);
//                                        }
//
//                                        Hotseat::instance.seatUser(guest.userID, guest.name);
//                                    }
//
//                                    gi->owner.guest.copy(_hosting.getGuests()[guestIndex]);
//                                }
//                            }
//                        }
//                    }
//                    else if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Gamepad"))
//                    {
//                        if (payload->DataSize == sizeof(int) && !Config::cfg.hotseat.enabled) {
//                            int sourceIndex = *(const int*)payload->Data;
//                            if (sourceIndex >= 0 && sourceIndex < _gamepads.size()) {
//                                static GuestDevice backupOwner;
//                                backupOwner.copy(_gamepads[i]->owner);
//
//                                Guest& guest = _gamepads[sourceIndex]->owner.guest;
//
//                                _gamepads[i]->copyOwner(_gamepads[sourceIndex]);
//                                _gamepads[sourceIndex]->owner.copy(backupOwner);
//                                _gamepads[i]->clearState();
//                                _gamepads[sourceIndex]->clearState();
//    
//                            }
//                        }
//                    }
//                }
//
//                ImGui::EndDragDropTarget();
//            }
//        }
//
//        ImGui::SetCursorPos(backupCursor);
//
//        if (gi->isConnected()) {
//
//            ImGui::Dummy(ImVec2(0, 8));
//
//            ImGui::Indent(20.0f);
//
//            if (IconButton::render(AppIcons::back, AppColors::primary, ImVec2(24, 24))) {
//
//                if (!_hosting.getGamepadClient().isSlave) {
//                    gi->clearOwner();
//                }
//
//            }
//            TitleTooltipWidget::render("Strip gamepad", "Unlink current user from this gamepad.");
//
//            ImGui::SameLine();
//
//            if (ToggleIconButtonWidget::render(AppIcons::padOn, AppIcons::padOff, gi->isConnected(), ImVec2(24, 24)))
//            {
//                if (!_hosting.getGamepadClient().isSlave) {
//                    if (gi->isConnected()) {
//                        gi->disconnect();
//                    }
//                    else {
//                        gi->connect();
//                    }
//
//                    isConnectionButtonPressed = true;
//                }
//            }
//            if (gi->isConnected()) TitleTooltipWidget::render("Connected gamepad", "Press to \"physically\" disconnect\nthis gamepad (at O.S. level).");
//            else                  TitleTooltipWidget::render("Disconnected gamepad", "Press to \"physically\" connect\nthis gamepad (at O.S. level).");
//
//            ImGui::SameLine();
//
//            if (gi->isLocked())
//            {
//                if (IconButton::render(AppIcons::lock, theme->negative, ImVec2(24, 24)))
//                {
//                    if (!_hosting.getGamepadClient().isSlave) {
//                        gi->toggleLocked();
//                    }
//                }
//                TitleTooltipWidget::render("Locked gamepad", "Unlock this specific gamepad, allowing inputs and picking.");
//            }
//            else
//            {
//                if (IconButton::render(AppIcons::unlock, theme->positive, ImVec2(24, 24)))
//                {
//                    if (!_hosting.getGamepadClient().isSlave) {
//                        gi->toggleLocked();
//                    }
//                }
//                TitleTooltipWidget::render("Unlocked gamepad", "Lock this specific gamepad, preventing inputs or picking.");
//            }
//
//            ImGui::SameLine();
//
//            if (gi->isLockedButtons())
//            {
//                if (IconButton::render(AppIcons::buttonLock, theme->negative, ImVec2(24, 24)))
//                {
//                    if (!_hosting.getGamepadClient().isSlave) {
//                        gi->toggleLockedButtons();
//                    }
//                }
//                TitleTooltipWidget::render("Unlock buttons", "Make sure you set it up");
//            }
//            else
//            {
//                if (IconButton::render(AppIcons::buttonLock, theme->positive, ImVec2(24, 24)))
//                {
//                    if (!_hosting.getGamepadClient().isSlave) {
//                        gi->toggleLockedButtons();
//                    }
//                }
//                TitleTooltipWidget::render("Lock buttons", "Make sure you set it up");
//            }
//
//            ImGui::SameLine();
//
//            ImGui::BeginGroup();
//
//            ImGui::Indent(10.0f);
//
//            if (!_hosting.getGamepadClient().isSlave) {
//                if (_hosting.getGamepadClient().isPuppetMaster && gi->isPuppet)
//                {
//                    ImGui::Dummy(ImVec2(0, 5.0f));
//                    ImGui::Image(AppIcons::puppet, ImVec2(35, 35), ImVec2(0, 0), ImVec2(1, 1), AppColors::primary);
//                    TitleTooltipWidget::render("Puppet", "This gamepad is under control of Master of Puppets.");
//                }
//                else
//                {
//                    static int deviceIndices[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
//                    ImGui::Dummy(ImVec2(0, 5.0f));
//                    ImGui::SetNextItemWidth(40);
//                    deviceIndices[i] = gi->owner.deviceID;
//
//                    AppFonts::pushTitle();
//                    if (ImGui::DragInt(
//                        (string("##DeviceIndex") + to_string(i)).c_str(),
//                        &deviceIndices[i], 0.1f, -1, 65536
//                    ))
//                    {
//                        gi->owner.deviceID = deviceIndices[i];
//                    }
//                    if (ImGui::IsItemHovered()) ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
//                    AppFonts::pop();
//
//                    TitleTooltipWidget::render("Device index", "A guest may have multiple gamepads in the same machine.");
//                }
//            }
//
//            ImGui::EndGroup();
//        } else {
//            ImGui::BeginGroup();
//                ImGui::Indent(20);
//                //ImGui::Dummy(ImVec2(0, 20.0f));
//                AppColors::pushButtonSolid();
//                if (ImGui::Button("Connect Gamepad")) {
//                    gi->connect();
//                    isConnectionButtonPressed = true;
//                }
//                ImGui::PopStyleColor(4);
//                //AppColors::pushButton();
//                ImGui::Unindent(20);
//            ImGui::EndGroup();
//        }
//
//        ImGui::EndChild();
//        
//        //ImGui::SameLine();
//        
//        
//        ImGui::EndChild();
//
//        //ImGui::Dummy(ImVec2(0, 2));
//
//        ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 10);
//
//        ImGui::BeginGroup();
//        ImGui::Dummy(ImVec2(48, 0));
//        
//        ImGui::SameLine();
//
//        //static AnimatedGamepadWidget agw;
//        //AnimatedGamepadWidget::render(gi->getState().Gamepad, 30);
//        if (gi->isConnected()) {
//            agw->render(gi->getState().Gamepad, 30);
//        }
//
//        ImGui::EndGroup();
//
//        ImGui::Dummy(ImVec2(0, 10));
//    }
//
//    ImGui::PopStyleVar();
//    AppStyle::pop();
//    ImGui::End();
//    AppStyle::pop();
//
//    ImGui::PopStyleVar();
//
//    static Stopwatch resizeGamepadsDebouncer = Stopwatch(500);
//
//    if (refreshGamepads || (Config::cfg.input.autoIndex && isConnectionButtonPressed)) {
//        refreshGamepads = false;
//        isConnectionButtonPressed = false;
//        for (size_t i = 0; i < _gamepads.size(); ++i)
//        {
//            _gamepads[i]->refreshIndex();
//        }
//    }
//
//    if (lastXboxCount != xboxCount || lastDs4Count != ds4Count)
//    {
//        if (xboxCount > 32) xboxCount = 32;
//        if (ds4Count > 32) ds4Count = 32;
//        lastXboxCount = xboxCount;
//        lastDs4Count = ds4Count;
//        Config::cfg.input.xboxPuppetCount = xboxCount;
//        Config::cfg.input.ds4PuppetCount = ds4Count;
//        _hosting.getGamepadClient().resize(xboxCount, ds4Count);
//        resizeGamepadsDebouncer.start();
//        resizeGamepadsDebouncer.reset(500);
//    }
//    if (resizeGamepadsDebouncer.isRunning() && resizeGamepadsDebouncer.isFinished())
//    {
//        resizeGamepadsDebouncer.stop();
//        Config::cfg.Save();
//    }
//
//    return true;
//}

void GamepadsWidget::renderPadInputTypeIcon(AGamepad* pad, const size_t& gamepadIndex, bool& refreshGamepads)
{
    static ImVec2 cursor;
    static const ImVec2 ICON_SIZE = ImVec2(45, 45);
    const static vector<Icon> xboxIcons{ AppIcons::xbox1, AppIcons::xbox2, AppIcons::xbox3, AppIcons::xbox4 };

    cursor = ImGui::GetCursorPos();
    static int xboxIndex = 0, padIndex = 0;
    xboxIndex = (int)pad->getIndex();
    padIndex = xboxIndex + 1;
    static bool isIndexSuccess = false;
    isIndexSuccess = pad->isConnected() && padIndex > 0 && padIndex <= 4;

    ImGui::BeginGroup();

    switch (pad->type())
    {
    case AGamepad::Type::XBOX:
        if (isIndexSuccess)
        {
			ImGui::Dummy(ImVec2(0, 2.0f));
			ImGui::Indent(2.0f);
            ImGui::Image(
                AppIcons::xbox,
                ICON_SIZE, ImVec2(0, 0), ImVec2(1, 1), AppColors::buttonSecondary
            );
			ImGui::Unindent(2.0f);

            ImGui::SetCursorPos(ImVec2(cursor.x-1, cursor.y+2));
            if (IconButton::render(xboxIcons[xboxIndex], AppColors::navbarIconActive, ICON_SIZE)) {
                refreshGamepads = true;
            }
            TitleTooltipWidget::render(
                (string() + "XInput " + to_string(padIndex)).c_str(),
                (
                    string("")
                    + "Press to identify XInput indices (this is visual only).\n\n"
                    + "This controller is using XInput slot " + to_string(padIndex) + ".\n" +
                    "* Remember:\nYour physical controllers may also occupy XInput slots."
                    ).c_str()
            );

        }
        else
        {
            if (IconButton::render(AppIcons::xinput, AppColors::formInputBackground, ICON_SIZE))
            {
                refreshGamepads = true;
            }
            TitleTooltipWidget::render(
                (string() + "XBox controller").c_str(),
                (
                    string("") +
                    +"Press to identify XInput indices (this is visual only).\n\n"
                    + "This is an XBox controller out of XInput range." + "\n" +
                    "* It still works, but as a generic controller without XInput features."
                    ).c_str()
            );
        }
        break;
    case AGamepad::Type::DUALSHOCK:
        pos = ImGui::GetCursorScreenPos();
        ImGui::SetCursorPos(ImVec2(cursor.x + 5, cursor.y));
        ImGui::Image(
            AppIcons::dinput,
            ICON_SIZE, ImVec2(0, 0), ImVec2(1, 1), (pad->isConnected() ? AppColors::buttonSecondary : AppColors::formInputBackground)
        );
        TitleTooltipWidget::render(
            (string() + "Dualshock controller").c_str(),
            (
                string("This is a Dualshock 4 controller.")).c_str()
        );
        break;
    default:
        break;
    }
    ImGui::EndGroup();
}


