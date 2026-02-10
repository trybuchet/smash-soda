#pragma once

#include "imgui.h"
#include <string>
#include <vector>
#include "../globals/AppColors.h"
#include "../globals/AppFonts.h"
#include "IntRangeWidget.h"
#include "../services/ThemeController.h"
#include "../models/Theme.h"
#include "../structs/Tab.h"


struct AutoWrapContext {
    float wrapWidth;
};

class Widget {
public:
    char* widgetName;
    bool isFocused = false;
    bool isHeaderFocused = false;
    bool isTitleFocused = false;
    bool isBodyFocused = false;
    bool isFooterFocused = false;
    bool isPopupOpen = false;
    bool hasHeader = false;
    bool hasFooter = false;
    int activeTab = 0;
    int headerHeight = 0;
    ImVec2 size = ImVec2(0, 0);
    ImVec2 pos = ImVec2(0, 0);

    void startWidget(const char* name, bool& visible, int x, int y, int w, int h, int minW = 350, int minH = 350);
    void startTabs(const std::vector<Tab>& tabs, bool footer = false);
    void endTabs();
    void startHeader(std::string id, int height);
    void endHeader();
    void startBody(bool footer = false);
    void endBody();
    void startFooter();
    void endFooter();
    void endWidget();

    void elLabel(std::string label);
    void elHelp(std::string help);
    void elError(std::string error);
    void elParagraph(std::string text);

    bool elText(
        std::string label,
        char* buffer,
        std::string help = "",
        std::string error = "",
        int bufferSize = 256,
        float width = -1.0f,
        ImGuiInputTextFlags flags = 0
    );
    void elReadOnly(std::string label, char* buffer, std::string help = "", std::string error = "");
    bool elPassword(std::string label, char* buffer, std::string help = "", std::string error = "");

    bool elSelect(std::string label, std::vector<std::pair<std::string, std::string>> options,
		string& callback, std::string help = "", std::string error = "");

    bool elMultiSelect(
        const std::string& label,
        const std::vector<std::pair<std::string, std::string>>& options,
        std::vector<std::string>& selectedValues,
        size_t limit,
        const std::string& help,
        const std::string& error
    );

    bool elTextArea(std::string label, char* buffer, std::string help = "", std::string error = "");
    bool elCheckbox(std::string label, bool& isOn, std::string help = "", std::string error = "");
    bool elNumber(std::string label, int& value, int from, int to, std::string help = "", std::string error = "");

    static bool elBtn(std::string label);
    static bool elBtnSecondary(std::string label);

    static int AutoWrapCallback(ImGuiInputTextCallbackData* data);
};
