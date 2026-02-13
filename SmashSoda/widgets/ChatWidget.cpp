#include "ChatWidget.h"

ChatWidget::ChatWidget(Hosting& hosting, function<void(void)> onMessageCallback)
    : _hosting(hosting), _chatLog(hosting.getMessageLog()), _messageCount(0), _onMessageCallback(onMessageCallback)
{
    setSendBuffer("\0");
}

bool ChatWidget::render(bool& showWindow) {

    Theme* theme = ThemeController::getInstance().getActiveTheme();

    static bool isClearChat = false;
    static bool isWindowLocked = false;
    static Stopwatch stopwatch = Stopwatch(2000);
    stopwatch.start();

    startWidget("Chat", showWindow, 62, 10, 800, 400, 350, 250);

    startBody(true);

        ImVec2 cursor;

        if (_chatLog.size() > CHATLOG_MESSAGE_LENGTH) {
            vector<string>::iterator it = _chatLog.begin();
            _chatLog.erase(it, it + CHATLOG_MESSAGE_LENGTH/2);
        }

        ImGui::BeginChild("Chat Log", ImVec2(0, size.y - 85));
        const float previousScrollY = ImGui::GetScrollY();
        const float previousScrollMaxY = ImGui::GetScrollMaxY();
        const bool wasNearBottom = previousScrollY >= (previousScrollMaxY - 10.0f);

        for (size_t i = 0; i < _chatLog.size(); ++i) {
            static float textHeight;
            cursor = ImGui::GetCursorPos();
        
            ImGui::TextWrapped(_chatLog[i].c_str());
            textHeight = ImGui::GetCursorPosY() - cursor.y - 4;

            ImGui::SetCursorPos(cursor);
            if (ImGui::InvisibleButton(
                (string() + "### Chat Message " + to_string(i)).c_str(),
                ImVec2(ImGui::GetContentRegionAvail().x, textHeight)
            )) {
                toClipboard(_chatLog[i]);
                stopwatch.reset();
            }
        }
        if (_messageCount != _chatLog.size()) {
            if (wasNearBottom) {
                ImGui::SetScrollHereY(1.0f);
            }
            _messageCount = _chatLog.size();

            if (_onMessageCallback != nullptr) {
                _onMessageCallback();
            }
        }
        ImGui::EndChild();

    endBody();

    startFooter();
	    ImGui::Dummy(ImVec2(10.0, .05f));

        if (_focusSendInputNextFrame && ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows)) {
            ImGui::SetKeyboardFocusHere();
            _focusSendInputNextFrame = false;
        }

        try {
            strcpy_s(_lastBuffer, SEND_BUFFER_LEN, _sendBuffer);
        }
        catch (const std::exception&) {}
        if (elText("", _sendBuffer, "", "", SEND_BUFFER_LEN, size.x - 90, ImGuiInputTextFlags_EnterReturnsTrue)) {
            sendMessage();
        }
        if (strcmp(_sendBuffer, _lastBuffer) != 0) {
            string newSendBuffer = _sendBuffer;
            Stringer::replacePatternOnce(newSendBuffer, "%", "%%");

            string check = newSendBuffer.substr(0, SEND_BUFFER_LEN-1);

            try {
                strcpy_s(_previewBuffer, SEND_BUFFER_LEN, newSendBuffer.substr(0, SEND_BUFFER_LEN-1).c_str());
            }
            catch (const std::exception&) {}
        }

        ImGui::SameLine();

        if (ToggleIconButtonWidget::render(
            AppIcons::send, AppIcons::send, isDirty(),
            theme->navbarIconActive, theme->navbarIconActive,
            ImVec2(20, 20)
        )) {
            sendMessage();
        }

        ImGui::SameLine();

        renderTopBar(isWindowLocked, isClearChat);

    endFooter();

    endWidget();

    return true;
}

//bool ChatWidget::render(bool &showWindow) {
//    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10, 5));
//
//    static bool isClearChat = false;
//    static bool isWindowLocked = false;
//    static Stopwatch stopwatch = Stopwatch(2000);
//    stopwatch.start();
//
//    AppStyle::pushTitle();
//    ImGui::SetNextWindowPos(ImVec2(45, 5), ImGuiCond_FirstUseEver);
//    ImGui::SetNextWindowSizeConstraints(ImVec2(400, 400), ImVec2(800, 900));
//    ImGui::Begin("Chat", &showWindow, isWindowLocked ? (ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize) : 0);
//    if (!showWindow) Config::cfg.widgets.chat = showWindow;
//    AppStyle::pushInput();
//
//    static ImVec2 size;
//    size = ImGui::GetContentRegionAvail();
//
//    ImVec2 cursor;
//
//    if (_chatLog.size() > CHATLOG_MESSAGE_LENGTH)
//    {
//        vector<string>::iterator it = _chatLog.begin();
//        _chatLog.erase(it, it + CHATLOG_MESSAGE_LENGTH/2);
//    }
//
//    renderTopBar(isWindowLocked, isClearChat);
//    ImGui::Separator();
//
//    ImGui::BeginChild("Chat Log", ImVec2(size.x, size.y - 85));
//    for (size_t i = 0; i < _chatLog.size(); ++i)
//    {
//        static float textHeight;
//        cursor = ImGui::GetCursorPos();
//        
//        ImGui::TextWrapped(_chatLog[i].c_str());
//        textHeight = ImGui::GetCursorPosY() - cursor.y - 4;
//
//        ImGui::SetCursorPos(cursor);
//        if (ImGui::Button(
//            (string() + "### Chat Message " + to_string(i)).c_str(),
//            ImVec2(size.x, textHeight)
//        ))
//        {
//            toClipboard(_chatLog[i]);
//            stopwatch.reset();
//        }
//    }
//    if (_messageCount != _chatLog.size())
//    {
//        if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY() - 100)
//        {
//            ImGui::SetScrollHereY(1.0f);
//        }
//        _messageCount = _chatLog.size();
//
//        if (_onMessageCallback != nullptr)
//        {
//            _onMessageCallback();
//        }
//    }
//    ImGui::EndChild();
//
//    //ImGui::BeginChild("Message Preview", ImVec2(size.x, 60));
//    //ImGui::Separator();
//    //ImGui::TextWrapped(_previewBuffer);
//    //ImGui::EndChild();
//
//    ImGui::Dummy(ImVec2(0, 1.0f));
//
//    ImGui::SetNextItemWidth(size.x);
//    try
//    {
//        strcpy_s(_lastBuffer, SEND_BUFFER_LEN, _sendBuffer);
//    }
//    catch (const std::exception&) {}
//    if (ImGui::InputText(" ", _sendBuffer, SEND_BUFFER_LEN, ImGuiInputTextFlags_EnterReturnsTrue))
//    {
//        sendMessage();
//    }
//    if (strcmp(_sendBuffer, _lastBuffer) != 0)
//    {
//        string newSendBuffer = _sendBuffer;
//        Stringer::replacePatternOnce(newSendBuffer, "%", "%%");
//
//        string check = newSendBuffer.substr(0, SEND_BUFFER_LEN-1);
//
//        try
//        {
//            strcpy_s(_previewBuffer, SEND_BUFFER_LEN, newSendBuffer.substr(0, SEND_BUFFER_LEN-1).c_str());
//        }
//        catch (const std::exception&) {}
//    }
//
//    if (ImGui::IsWindowFocused() && !ImGui::IsAnyItemActive() && !ImGui::IsMouseClicked(0))
//        ImGui::SetKeyboardFocusHere(-1);
//    // Activate chat when not foreground window
//    if (GetForegroundWindow() != _hosting.mainWindow && !ImGui::IsItemActive()) {
//        ImGui::SetKeyboardFocusHere(-1);
//    }
//
//    //cursor = ImGui::GetCursorPos();
//    //ImGui::Dummy(ImVec2(0, 5));
//    //if (_chatLog.size() > 0 && stopwatch.getRemainingTime() > 0)
//    //{
//    //    static float fill = 1.0f;
//    //    fill = (float)stopwatch.getRemainingTime() / stopwatch.getDuration();
//    //    ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, fill*fill), "Message copied.");
//    //}
//
//    //ImGui::SetCursorPos(cursor);
//
//    //ImGui::Indent(size.x - 50);
//
//    //if (ToggleIconButtonWidget::render(
//    //    AppIcons::send, AppIcons::send, isDirty(),
//    //    AppColors::primary, AppColors::alpha(AppColors::primary, 0.25f)
//    //))
//    //{
//    //    sendMessage();
//    //}
//
//    AppStyle::pop();
//    ImGui::End();
//    AppStyle::pop();
//
//    ImGui::PopStyleVar();
//
//    if (isClearChat)
//    {
//        _chatLog.clear();
//        ChatBot* chatBot = _hosting.getChatBot();
//        if (chatBot != nullptr) {
//            chatBot->setLastUserId();
//        }
//        isClearChat = false;
//    }
//
//    return true;
//}

bool ChatWidget::renderTopBar(bool& isWindowLocked, bool& isClearChat) {
    static ImVec2 cursor;
    static bool result;
    result = false;

    Theme* theme = ThemeController::getInstance().getActiveTheme();

    static bool isDeletingChat = false;
    if (!isDeletingChat) {
        if (IconButton::render(AppIcons::trash, theme->buttonSecondary, ImVec2(20, 20))) {
            //isDeletingChat = true;
            isClearChat = true;
            result = true;
        }
        TitleTooltipWidget::render("Clear Chat", "Deletes all chat messages.");
    } else {
        ImGui::BeginGroup();

        ImGui::PushFont(AppFonts::input);
        ImGui::PushStyleColor(ImGuiCol_Text, theme->formInputText);
        ImGui::Text("Delete chat history?");
        ImGui::PopStyleColor();
        ImGui::PopFont();

        static bool btnYes, btnNo;

        btnNo = IconButton::render(AppIcons::no, theme->negative, ImVec2(20, 20));
        ImGui::SameLine();
        btnYes = IconButton::render(AppIcons::yes, theme->positive, ImVec2(20, 20));

        if (btnYes) {
            isClearChat = true;
            isDeletingChat = false;
            result = true;
        }
        else if (btnNo)
        {
            isDeletingChat = false;
        }

        ImGui::EndGroup();
    }

    //ImGui::SameLine();
    //cursor = ImGui::GetCursorPos();
    //ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 40);
    //if (IconButton::render(
    //    AppIcons::move,
    //    isWindowLocked ? AppColors::negative : AppColors::positive,
    //    ImVec2(30, 30)
    //))
    //{
    //    isWindowLocked = !isWindowLocked;
    //}
    //if (isWindowLocked) TitleTooltipWidget::render("Window Locked", "This window cannot move or resize.");
    //else TitleTooltipWidget::render("Window Unlocked", "This window can move and resize.");

    return result;
}

bool ChatWidget::isDirty()
{
    static size_t len;
    try
    {
        len = strnlen_s(_sendBuffer, SEND_BUFFER_LEN);
    }
    catch (const std::exception&)
    {
        len = 0;
    }
    return len > 0;
}


void ChatWidget::sendMessage() {
    /*if (_hosting.isRunning() && strlen(_sendBuffer) > 0) {
        _hosting.sendHostMessage(_sendBuffer);
    }*/
    if (strlen(_sendBuffer) > 0) {
        _hosting.sendHostMessage(_sendBuffer);
    }

    setSendBuffer("\0");
    _focusSendInputNextFrame = true;
}

bool ChatWidget::setSendBuffer(const char* value)
{
    try
    {
        strcpy_s(_sendBuffer, SEND_BUFFER_LEN, value);
        strcpy_s(_lastBuffer, SEND_BUFFER_LEN, value);
        strcpy_s(_previewBuffer, SEND_BUFFER_LEN, value);
        return true;
    }
    catch (const std::exception&)
    {
        return false;
    }

    return false;
}

void ChatWidget::toClipboard(const string& message)
{
    string adjustedMessage = message;

    if (Stringer::startsWithPattern(adjustedMessage.c_str(), "\t\t "))
    {
        Stringer::replacePatternOnce(adjustedMessage, "\t\t ", "");
    }

    OpenClipboard(0);
    EmptyClipboard();
    HGLOBAL hg = GlobalAlloc(GMEM_MOVEABLE, adjustedMessage.size()+1);
    if (!hg) {
        CloseClipboard();
        return;
    }
    memcpy(GlobalLock(hg), adjustedMessage.c_str(), adjustedMessage.size());
    GlobalUnlock(hg);
    SetClipboardData(CF_TEXT, hg);
    CloseClipboard();
    GlobalFree(hg);
}



