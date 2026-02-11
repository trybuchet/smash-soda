#include "Widget.h"
#include <algorithm>

int Widget::AutoWrapCallback(ImGuiInputTextCallbackData* data) {

    AutoWrapContext* ctx = static_cast<AutoWrapContext*>(data->UserData);

    ImFont* font = ImGui::GetFont();
    float fontSize = ImGui::GetFontSize();

    int cursor = data->CursorPos;

    int lineStart = cursor;
    while (lineStart > 0 && data->Buf[lineStart - 1] != '\n') {
        lineStart--;
    }

    int lineEnd = lineStart;
    while (lineEnd < data->BufTextLen && data->Buf[lineEnd] != '\n') {
        lineEnd++;
    }

    float lineWidth = font->CalcTextSizeA(
        fontSize,
        FLT_MAX,
        0.0f,
        data->Buf + lineStart,
        data->Buf + lineEnd
    ).x;

    if (lineWidth <= ctx->wrapWidth) {
        return 0;
    }

    int wrapPos = cursor;
    while (wrapPos > lineStart && data->Buf[wrapPos - 1] != ' ') {
        wrapPos--;
    }

    if (wrapPos <= lineStart) {
        return 0;
    }

    data->InsertChars(wrapPos, "\n");

    if (cursor >= wrapPos) {
        data->CursorPos++;
    }

    return 0;
}


/**
 * Set the position and dimensions.
 *
 * \param x	The x coordinate.
 * \param y	The y coordinate.
 * \param w	The width.
 * \param h The height.
 */
void Widget::startWidget(const char* name, bool& visible, int x, int y, int w, int h, int minW, int minH) {

    // Set widget name
    widgetName = (char*)name;

    Theme* theme = ThemeController::getInstance().getActiveTheme();
    bool wasPopupOpen = isPopupOpen;
    isPopupOpen = false;

    // Check if the window is focused, and set text color accordingly
    if (isTitleFocused || isHeaderFocused || isBodyFocused || isFooterFocused || isFocused || wasPopupOpen) {
        ImGui::PushStyleColor(ImGuiCol_Text, theme->panelTitleBarActiveText);  // Focused text color
        ImGui::PushStyleColor(ImGuiCol_Border, theme->panelBorderActive);
        ImGui::PushStyleColor(ImGuiCol_TitleBg, theme->panelTitleBarActive);
        ImGui::PushStyleColor(ImGuiCol_TitleBgCollapsed, theme->panelTitleBarActive);
    } else {
        ImGui::PushStyleColor(ImGuiCol_Text, theme->panelTitleBarText);
        ImGui::PushStyleColor(ImGuiCol_Border, theme->panelBorder);
        ImGui::PushStyleColor(ImGuiCol_TitleBg, theme->panelTitleBar);
        ImGui::PushStyleColor(ImGuiCol_TitleBgCollapsed, theme->panelTitleBar);
    }

    ImGui::PushStyleColor(ImGuiCol_WindowBg, theme->panelBackground);
    ImGui::PushStyleColor(ImGuiCol_FrameBgActive, theme->panelBackground);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, theme->panelBackground);
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, theme->panelBackground);
    ImGui::PushStyleColor(ImGuiCol_FrameBgActive, theme->panelBackground);

    // Set the font
    ImGui::PushFont(AppFonts::title);

    // Set window position and size constraints
    ImGui::SetNextWindowPos(ImVec2(x, y), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSizeConstraints(ImVec2(minW, minH), ImVec2(w, h));

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 8);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 8);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0,0,0,0.2f));

    ImGui::Begin(name, &visible, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

    size = ImGui::GetContentRegionAvail();
    pos = ImGui::GetWindowPos();

    // Is ImGui::Begin() focused? (include child windows)
    isTitleFocused = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows);

    // Reset style
    ImGui::PopStyleVar(3);
    ImGui::PopStyleColor();
}

void Widget::endTabs() {
    ImGui::Unindent(20);
    ImGui::EndTabBar();
}

/**
 * Tabs header
 */
void Widget::startTabs(const std::vector<Tab>& tabs, bool footer) {
    hasFooter = footer;

    Theme* theme = ThemeController::getInstance().getActiveTheme();

    ImGui::SetCursorPos(ImVec2(0, 0));
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    pos = ImGui::GetCursorScreenPos();
    drawList->AddRectFilled(
        pos,
        ImVec2(pos.x + size.x, pos.y + 64),
        ImGui::ColorConvertFloat4ToU32(theme->tabsBackground),
        10,
        ImDrawFlags_RoundCornersBottom
    );

    ImGui::SetCursorPos(ImVec2(0, 0));
    ImGui::Dummy(ImVec2(0, 30));

    std::string pillsName = "##pills-" + std::string(widgetName);
    ImGui::BeginTabBar(pillsName.c_str());

    ImGui::Dummy(ImVec2(20, 10));
    ImGui::Indent(20);

    for (int i = 0; i < tabs.size(); i++) {

        if (activeTab == i) {
            ImGui::PushStyleColor(ImGuiCol_Button, theme->buttonPrimary);
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, theme->buttonPrimaryHovered);
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, theme->buttonPrimaryActive);
            ImGui::PushStyleColor(ImGuiCol_Text, theme->buttonPrimaryText);
        } else {
            ImGui::PushStyleColor(ImGuiCol_Button, theme->buttonSecondary);
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, theme->buttonSecondaryHovered);
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, theme->buttonSecondaryActive);
            ImGui::PushStyleColor(ImGuiCol_Text, theme->buttonSecondaryText);
        }

        if (ImGui::BeginTabItem(tabs[i].name)) {
            activeTab = i;
            tabs[i].render();
            ImGui::EndTabItem();
        }
        ImGui::PopStyleColor(4);

    }
}

/**
 * Start widget header.
 *
 */
void Widget::startHeader(string id, int height) {

	headerHeight = height;
    size = ImGui::GetContentRegionAvail();
    pos = ImGui::GetCursorScreenPos();
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 10);
    ImGui::BeginChild(id.c_str(), ImVec2(size.x, 50), false, ImGuiWindowFlags_NoScrollbar);
    ImGui::SetCursorPos(ImVec2(0, 0));
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    pos = ImGui::GetCursorScreenPos();
    drawList->AddRectFilled(pos, ImVec2(pos.x + size.x, pos.y + headerHeight), ImGui::ColorConvertFloat4ToU32(ImVec4(0, 0, 0, 0.1)), 8, ImDrawFlags_RoundCornersBottom);
    ImGui::SetCursorPos(ImVec2(0, 0));
    ImGui::Dummy(ImVec2(0, 2));
    ImGui::Indent(10);
	isHeaderFocused = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows);
    hasHeader = true;

}

/**
 * Stop widget header.
 *
 */
void Widget::endHeader() {

    ImGui::Unindent(10);
    ImGui::SetCursorPos(ImVec2(0, 0));
    ImGui::SetCursorPos(ImVec2(0, headerHeight-1));
    ImGui::Separator();
    ImGui::EndChild();
    ImGui::PopStyleVar();

}

void Widget::startBody(bool footer) {
    hasFooter = footer;

    Theme* theme = ThemeController::getInstance().getActiveTheme();

    if (hasFooter) {
    }
    ImGui::BeginChild("##body", ImVec2(size.x, (footer ? size.y-58 : size.y)));
    ImGui::Dummy(ImVec2(20, 10));  // Adds top-left padding
    ImGui::Indent(20);
    isBodyFocused = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows);
    ImGui::PushStyleColor(ImGuiCol_ChildBg, theme->panelBackground);
    ImGui::PushStyleColor(ImGuiCol_Text, theme->panelText);
}

void Widget::endBody() {
    ImGui::Unindent(20);
	ImGui::EndChild();
    ImGui::PopStyleColor();
	ImGui::PopStyleColor();
}

/**
 * Start widget footer.
 *
 */
void Widget::startFooter() {
    Theme* theme = ThemeController::getInstance().getActiveTheme();
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    size = ImGui::GetContentRegionAvail();
    ImGui::BeginChild("##footer", ImVec2(size.x, 58), false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImDrawFlags_RoundCornersBottom);
    isFooterFocused = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows);
    ImGui::SetCursorPos(ImVec2(0, 0));
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    pos = ImGui::GetCursorScreenPos();
    drawList->AddRectFilled(pos, ImVec2(pos.x + size.x, pos.y + 52), ImGui::ColorConvertFloat4ToU32(theme->panelFooter), 10, ImDrawFlags_RoundCornersBottom);
	ImGui::PopStyleVar();
	ImGui::Separator();
	ImGui::SetCursorPos(ImVec2(10, 10));
    ImGui::Indent(10);
}

/**
 * Stop widget footer.
 *
 */
void Widget::endFooter() {
    ImGui::Unindent(10);
    ImGui::EndChild();
}

/**
 * End the widget.
 *
 */
void Widget::endWidget() {
    isFocused = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) || isPopupOpen;
	ImGui::End();
    ImGui::PopFont();
    ImGui::PopStyleColor(9);
}

void Widget::elLabel(std::string label) {

    Theme* theme = ThemeController::getInstance().getActiveTheme();

    ImGui::PushFont(AppFonts::label);
    ImGui::PushStyleColor(ImGuiCol_Text, theme->formLabel);
    ImGui::SetNextItemWidth(size.x - 40);
	ImGui::Text(label.c_str());
	ImGui::PopStyleColor();
    ImGui::PopFont();
}

void Widget::elHelp(std::string help) {

    if (help.empty()) return;

    Theme* theme = ThemeController::getInstance().getActiveTheme();

    ImGui::PushFont(AppFonts::input);
    ImGui::PushStyleColor(ImGuiCol_Text, theme->formHelpText);
    ImGui::SetNextItemWidth(size.x - 60);
    ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + size.x - 60);
	ImGui::TextWrapped(help.c_str());
    ImGui::PopStyleColor();
	ImGui::PopFont();
}

void Widget::elError(std::string error) {
    if (error.empty()) return;

    ImGui::PushFont(AppFonts::input);
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.3f, 0.3f, 1.0f));
    ImGui::SetNextItemWidth(size.x - 60);
    ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + size.x - 60);
    ImGui::TextWrapped(error.c_str());
    ImGui::PopTextWrapPos();
    ImGui::PopStyleColor();
    ImGui::PopFont();
}



void Widget::elParagraph(std::string text) {

    Theme* theme = ThemeController::getInstance().getActiveTheme();

    ImGui::PushFont(AppFonts::input);
    ImGui::PushStyleColor(ImGuiCol_Text, theme->panelText);
    ImGui::SetNextItemWidth(size.x - 40);
    ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + size.x - 60);
    ImGui::TextWrapped(text.c_str());
    ImGui::PopStyleColor();
    ImGui::PopFont();
    ImGui::Dummy(ImVec2(0, 10.0f));
}

bool Widget::elText(
    std::string label,
    char* buffer,
    std::string help,
    std::string error,
    int bufferSize,
    float width,
    ImGuiInputTextFlags flags
) {

    Theme* theme = ThemeController::getInstance().getActiveTheme();

    bool response = false;
    ImVec2 size = ImGui::GetContentRegionAvail();
    std::string inputLabel = "##" + label;

    if (!label.empty()) {
        elLabel(label);
	}
    ImGui::SetNextItemWidth(width > 0.0f ? width : size.x - 20);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, theme->formInputBackground);
    ImGui::PushStyleColor(ImGuiCol_Text, theme->formInputText);

    if (!error.empty()) {
        ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(1.0f, 0.3f, 0.3f, 1.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
    }

    if (ImGui::InputText(inputLabel.c_str(), buffer, bufferSize, flags)) {
        response = true;
    }
    else {
        response = false;
    }

    if (error.empty() && ImGui::IsItemActive()) {
        ImDrawList* drawList = ImGui::GetWindowDrawList();
        ImVec2 rectMin = ImGui::GetItemRectMin();
        ImVec2 rectMax = ImGui::GetItemRectMax();
        float rounding = ImGui::GetStyle().FrameRounding;
        drawList->AddRect(
            rectMin,
            rectMax,
            ImGui::ColorConvertFloat4ToU32(theme->formInputBorderActive),
            rounding,
            0,
            1.0f
        );
    }

    if (!error.empty()) {
        ImGui::PopStyleVar();
        ImGui::PopStyleColor();
    }

    ImGui::PopStyleColor(2);

    if (!help.empty() || !error.empty()) {
        ImGui::SetNextItemWidth(size.x - 20);
        elHelp(help);
        elError(error);
        ImGui::Dummy(ImVec2(0, 10.0f));
    }

    return response;

}

void Widget::elReadOnly(std::string label, char* buffer, std::string help, std::string error) {

    Theme* theme = ThemeController::getInstance().getActiveTheme();

    ImVec2 size = ImGui::GetContentRegionAvail();
    std::string inputLabel = "##" + label;

    if (!label.empty()) {
        elLabel(label);
    }
    ImGui::SetNextItemWidth(size.x - 20);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, theme->formInputBackground);
    ImGui::PushStyleColor(ImGuiCol_Text, theme->formInputText);
    ImGui::PushStyleColor(ImGuiCol_Border, theme->formInputBorderActive);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
    ImGui::InputText(inputLabel.c_str(), buffer, 256, ImGuiInputTextFlags_ReadOnly);
    ImGui::PopStyleVar();
    ImGui::PopStyleColor();
    ImGui::PopStyleColor();
    ImGui::PopStyleColor();

    ImGui::SetNextItemWidth(size.x - 20);
    elHelp(help);
    elError(error);

    if (!help.empty() || !error.empty()) {
        ImGui::Dummy(ImVec2(0, 10.0f));
    }
}



bool Widget::elPassword(std::string label, char* buffer, std::string help, std::string error) {
    
    Theme* theme = ThemeController::getInstance().getActiveTheme();

    bool response = false;
    ImVec2 size = ImGui::GetContentRegionAvail();
    std::string inputLabel = "##" + label;

    if (!label.empty()) {
        elLabel(label);
    }
    ImGui::SetNextItemWidth(size.x - 20);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, theme->formInputBackground);
    ImGui::PushStyleColor(ImGuiCol_Text, theme->formInputText);
    if (!error.empty()) {
        ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(1.0f, 0.3f, 0.3f, 1.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
    }

    if (ImGui::InputText(inputLabel.c_str(), buffer, 256, ImGuiInputTextFlags_Password)) {
        response = true;
    }
    else {
        response = false;
    }

    if (error.empty() && ImGui::IsItemActive()) {
        ImDrawList* drawList = ImGui::GetWindowDrawList();
        ImVec2 rectMin = ImGui::GetItemRectMin();
        ImVec2 rectMax = ImGui::GetItemRectMax();
        float rounding = ImGui::GetStyle().FrameRounding;
        drawList->AddRect(
            rectMin,
            rectMax,
            ImGui::ColorConvertFloat4ToU32(theme->formInputBorderActive),
            rounding,
            0,
            1.0f
        );
    }

    if (!error.empty()) {
        ImGui::PopStyleVar();
        ImGui::PopStyleColor();
    }

    ImGui::PopStyleColor(2);

    ImGui::SetNextItemWidth(size.x - 20);
    elHelp(help);

    elError(error);

    if (!help.empty() || !error.empty()) {
        ImGui::Dummy(ImVec2(0, 10.0f));
    }

    return response;

}

bool Widget::elNumber(std::string label, int& value, int from, int to, std::string help, std::string error) {
    
    Theme* theme = ThemeController::getInstance().getActiveTheme();

    bool response = false;
    ImVec2 size = ImGui::GetContentRegionAvail();
    std::string inputLabel = "##" + label;

    elLabel(label);
    ImGui::SetNextItemWidth(size.x-20);
    ImGui::PushStyleColor(ImGuiCol_Text, theme->formInputText);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, theme->formInputBackground);
    if (IntRangeWidget::render(label.c_str(), value, from, to, 0.025f)) {
        response = true;
    }
    else {
        response = false;
    }
    ImGui::PopStyleColor(2);

    ImGui::SetNextItemWidth(size.x-20);
    elHelp(help);

    elError(error);

    if (!help.empty() || !error.empty()) {
        ImGui::Dummy(ImVec2(0, 10.0f));
    }

    return response;

}

bool Widget::elTextArea(std::string label, char* buffer, std::string help, std::string error) {

    Theme* theme = ThemeController::getInstance().getActiveTheme();

    ImVec2 avail = ImGui::GetContentRegionAvail();
    std::string inputLabel = "##" + label;

    elLabel(label);

    ImVec2 boxSize(
        avail.x - 20,
        ImGui::GetTextLineHeight() * 6
    );

    AutoWrapContext ctx;
    ctx.wrapWidth = boxSize.x - ImGui::GetStyle().FramePadding.x * 2.0f;

    ImGui::PushStyleColor(ImGuiCol_Text, theme->formInputText);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, theme->formInputBackground);
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, theme->formInputBackground);
    ImGui::PushStyleColor(ImGuiCol_FrameBgActive, theme->formInputBackground);
    if (!error.empty()) {
        ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(1.0f, 0.3f, 0.3f, 1.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
    }

    bool changed = ImGui::InputTextMultiline(
        inputLabel.c_str(),
        buffer,
        500,
        boxSize,
        ImGuiInputTextFlags_NoHorizontalScroll |
        ImGuiInputTextFlags_CallbackEdit,
        Widget::AutoWrapCallback,
        &ctx
    );

    if (error.empty() && ImGui::IsItemActive()) {
        ImDrawList* drawList = ImGui::GetWindowDrawList();
        ImVec2 rectMin = ImGui::GetItemRectMin();
        ImVec2 rectMax = ImGui::GetItemRectMax();
        float rounding = ImGui::GetStyle().FrameRounding;
        drawList->AddRect(
            rectMin,
            rectMax,
            ImGui::ColorConvertFloat4ToU32(theme->formInputBorderActive),
            rounding,
            0,
            1.0f
        );
    }

    if (!error.empty()) {
        ImGui::PopStyleVar();
        ImGui::PopStyleColor();
    }
    ImGui::PopStyleColor(4);

    elHelp(help);
    elError(error);

    if (!help.empty() || !error.empty()) {
        ImGui::Dummy(ImVec2(0, 10.0f));
    }

    return changed;
}

bool Widget::elCheckbox(std::string label, bool& isOn, std::string help, std::string error) {
    
    Theme* theme = ThemeController::getInstance().getActiveTheme();

    bool response = false;
    ImVec2 size = ImGui::GetContentRegionAvail();

    ImGui::PushFont(AppFonts::label);
    ImGui::PushStyleColor(ImGuiCol_Text, theme->formLabel);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, theme->formInputBackground);
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, theme->formInputBackground);
    ImGui::PushStyleColor(ImGuiCol_FrameBgActive, theme->formInputBackground);

    ImGui::SetNextItemWidth(size.x-20);
    if (ImGui::Checkbox(label.c_str(), &isOn)) {
        response = true;
    }
    ImGui::PopStyleColor(4);
    ImGui::PopFont();

    ImGui::SetNextItemWidth(size.x-20);
    elHelp(help);

    elError(error);

    if (!help.empty() || !error.empty()) {
        ImGui::Dummy(ImVec2(0, 10.0f));
    }

    return response;

}

bool Widget::elSelect(std::string label,
    std::vector<std::pair<std::string, std::string>> options,
    string& callback, std::string help, std::string error) {

    Theme* theme = ThemeController::getInstance().getActiveTheme();

    ImVec2 size = ImGui::GetContentRegionAvail();
    std::string inputLabel = "##" + label;

    elLabel(label);
    ImGui::SetNextItemWidth(size.x-20);
    ImGui::PushStyleColor(ImGuiCol_Text, theme->formInputText);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, theme->formInputBackground);
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, theme->formInputBackground);
    ImGui::PushStyleColor(ImGuiCol_FrameBgActive, theme->formInputBackground);

    bool itemSelected = false;

    // Get selected option
    std::string selected = "";
    for (size_t i = 0; i < options.size(); ++i) {
        if (options[i].first == callback) {
			selected = options[i].second.c_str();
			break;
		}
	}

    ImGui::PushStyleColor(ImGuiCol_Button, theme->buttonPrimary);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, theme->buttonPrimaryActive);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, theme->buttonPrimaryHovered);
    ImGui::PushStyleColor(ImGuiCol_Text, theme->formInputText);
    if (ImGui::BeginCombo(inputLabel.c_str(), selected.c_str(), ImGuiComboFlags_HeightLarge)) {
        isPopupOpen = true;
        for (size_t i = 0; i < options.size(); ++i) {
            bool isSelected = (options[i].first == callback);
            if (ImGui::Selectable(options[i].second.c_str(), isSelected)) {
                callback = options[i].first;
                itemSelected = true; // Store selection state
            }
            if (isSelected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }

    ImGui::PopStyleColor(8);

    ImGui::SetNextItemWidth(size.x-20);
    elHelp(help);

    elError(error);

    if (!help.empty() || !error.empty()) {
        ImGui::Dummy(ImVec2(0, 10.0f));
    }

    return itemSelected; // Return after everything is drawn
}

bool Widget::elMultiSelect(
    const std::string& label,
    const std::vector<std::pair<std::string, std::string>>& options,
    std::vector<std::string>& selectedValues,
    size_t limit,
    const std::string& help,
    const std::string& error
) {
    Theme* theme = ThemeController::getInstance().getActiveTheme();

    ImVec2 size = ImGui::GetContentRegionAvail();
    std::string inputLabel = "##" + label;

    elLabel(label);
    ImGui::SetNextItemWidth(size.x - 20);

    std::string preview;
    for (size_t i = 0; i < selectedValues.size(); ++i) {
        auto it = std::find_if(
            options.begin(),
            options.end(),
            [&](const auto& o) { return o.first == selectedValues[i]; }
        );
        if (it != options.end()) {
            if (!preview.empty()) preview += ", ";
            preview += it->second;
        }
    }
    if (preview.empty()) {
        preview = "Select...";
    }

    bool changed = false;

    ImGui::PushStyleColor(ImGuiCol_Text, theme->formInputText);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, theme->formInputBackground);
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, theme->formInputBackground);
    ImGui::PushStyleColor(ImGuiCol_FrameBgActive, theme->formInputBackground);

    if (ImGui::BeginCombo(inputLabel.c_str(), preview.c_str(), ImGuiComboFlags_HeightLarge)) {
        isPopupOpen = true;

        for (const auto& opt : options) {

            bool isSelected = std::find(
                selectedValues.begin(),
                selectedValues.end(),
                opt.first
            ) != selectedValues.end();

            if (ImGui::Selectable(
                    opt.second.c_str(),
                    isSelected,
                    ImGuiSelectableFlags_DontClosePopups
                )) {

                if (isSelected) {
                    selectedValues.erase(
                        std::remove(selectedValues.begin(), selectedValues.end(), opt.first),
                        selectedValues.end()
                    );
                    changed = true;
                }
                else if (selectedValues.size() < limit) {
                    selectedValues.push_back(opt.first);
                    changed = true;
                }
            }
        }

        ImGui::EndCombo();
    }

    ImGui::PopStyleColor(4);

    elHelp(help);
    elError(error);

    if (!help.empty() || !error.empty()) {
        ImGui::Dummy(ImVec2(0, 10.0f));
    }

    return changed;
}



bool Widget::elBtn(std::string label) {

    Theme* theme = ThemeController::getInstance().getActiveTheme();

    ImGui::PushStyleColor(ImGuiCol_Button, theme->buttonPrimary);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, theme->buttonPrimaryActive);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, theme->buttonPrimaryHovered);
    ImGui::PushStyleColor(ImGuiCol_Text, theme->buttonPrimaryText);

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 5));

    if (ImGui::Button(label.c_str())) {

        ImGui::PopStyleColor();
        ImGui::PopStyleColor();
        ImGui::PopStyleColor();
        ImGui::PopStyleColor();
        ImGui::PopStyleVar();
        if (ImGui::IsItemHovered()) ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

        return true;
    }

    ImGui::PopStyleColor();
    ImGui::PopStyleColor();
    ImGui::PopStyleColor();
    ImGui::PopStyleColor();
    ImGui::PopStyleVar();
    if (ImGui::IsItemHovered()) ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
    return false;
}

bool Widget::elBtnSecondary(std::string label) {
    
    Theme* theme = ThemeController::getInstance().getActiveTheme();

	ImGui::PushStyleColor(ImGuiCol_Button, theme->buttonSecondary);
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, theme->buttonSecondaryActive);
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, theme->buttonSecondaryHovered);
    ImGui::PushStyleColor(ImGuiCol_Text, theme->buttonSecondaryText);

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 5));
    if (ImGui::Button(label.c_str())) {

        ImGui::PopStyleColor();
        ImGui::PopStyleColor();
        ImGui::PopStyleColor();
        ImGui::PopStyleColor();
        ImGui::PopStyleVar();
        if (ImGui::IsItemHovered()) ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

		return true;
	}

    ImGui::PopStyleColor();
    ImGui::PopStyleColor();
    ImGui::PopStyleColor();
    ImGui::PopStyleColor();
    ImGui::PopStyleVar();
    if (ImGui::IsItemHovered()) ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
	return false;
}
