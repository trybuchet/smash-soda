#include "Theme.h"
#include "imgui/imgui.h"
#include <algorithm>
#include <cctype>
#include <vector>

namespace {
    std::string trim(const std::string& value) {
        const auto begin = std::find_if_not(value.begin(), value.end(), [](unsigned char c) {
            return std::isspace(c) != 0;
        });
        if (begin == value.end()) {
            return "";
        }

        const auto end = std::find_if_not(value.rbegin(), value.rend(), [](unsigned char c) {
            return std::isspace(c) != 0;
        }).base();

        return std::string(begin, end);
    }
}

Theme::Theme(const string& name, const json& theme)
    : name(name) {

    // Default to Ember palette so partial custom JSON themes remain valid.
    panelBackground = hexToImVec4("#302E2BFF");
    panelBorder = hexToImVec4("#FFFFFF33");
    panelBorderActive = hexToImVec4("#B04229FF");
    panelText = hexToImVec4("#F2EEDDFF");
    panelTitleBar = hexToImVec4("#F2EEDDFF");
    panelTitleBarText = hexToImVec4("#302E2BFF");
    panelTitleBarActive = hexToImVec4("#B04229FF");
    panelTitleBarActiveText = hexToImVec4("#FFFFFFFF");
    panelFooter = hexToImVec4("#282623FF");
    panelFooterText = hexToImVec4("#B3B3B3FF");

    text = hexToImVec4("#F2EEDDFF");
    textMuted = hexToImVec4("#B3B3B3FF");
    textHighlighted = hexToImVec4("#E8CC40FF");
    positive = hexToImVec4("#8CBF8CFF");
    negative = hexToImVec4("#CC7373FF");
    warning = textHighlighted;

    listItemBackground = hexToImVec4("#353330FF");

    tabsBackground = hexToImVec4("#282623FF");
    tabsBorder = hexToImVec4("#B04229FF");
    tab = hexToImVec4("#F2EEDDFF");
    tabText = hexToImVec4("#302E2BFF");
    tabHovered = hexToImVec4("#B5472EFF");
    tabActive = hexToImVec4("#B04229FF");
    tabActiveText = hexToImVec4("#FFFFFFFF");

    navbarBackground = hexToImVec4("#0F0F0FFF");
    navbarBorder = hexToImVec4("#262626FF");
    navbarIcon = hexToImVec4("#FFFFFF26");
    navbarIconActive = hexToImVec4("#D45639FF");

    formLabel = hexToImVec4("#E8A540FF");
    formInputBackground = hexToImVec4("#1A1A1AFF");
    formInputBorder = hexToImVec4("#262626FF");
    formInputBorderActive = hexToImVec4("#FFFFFFFF");
    formInputText = hexToImVec4("#B3B3B3FF");
    formInputTextActive = hexToImVec4("#FFFFFFFF");
    formHelpText = hexToImVec4("#F2EEDDFF");

    buttonPrimary = hexToImVec4("#B04229FF");
    buttonPrimaryBorder = hexToImVec4("#B04229FF");
    buttonPrimaryText = hexToImVec4("#FFFFFFFF");
    buttonPrimaryHovered = hexToImVec4("#B5472EFF");
    buttonPrimaryHoveredText = hexToImVec4("#FFFFFFFF");
    buttonPrimaryActive = hexToImVec4("#B8482EFF");
    buttonPrimaryActiveText = hexToImVec4("#FFFFFFFF");
    buttonPrimaryDisabled = hexToImVec4("#262626FF");

    buttonSecondary = hexToImVec4("#F2EEDDFF");
    buttonSecondaryBorder = hexToImVec4("#00000033");
    buttonSecondaryText = hexToImVec4("#302E2BFF");
    buttonSecondaryHovered = hexToImVec4("#FAF3D9FF");
    buttonSecondaryHoveredText = hexToImVec4("#302E2BFF");
    buttonSecondaryActive = hexToImVec4("#D4D4D4FF");
    buttonSecondaryActiveText = hexToImVec4("#FFFFFFFF");
    buttonSecondaryDisabled = hexToImVec4("#262626FF");

    buttonEnable = buttonPrimary;
    buttonEnableBorder = buttonPrimaryBorder;
    buttonEnableText = buttonPrimaryText;
    buttonEnableHovered = buttonPrimaryHovered;
    buttonEnableHoveredText = buttonPrimaryHoveredText;
    buttonEnableActive = buttonPrimaryActive;
    buttonEnableActiveText = buttonPrimaryActiveText;
    buttonEnableDisabled = buttonPrimaryDisabled;

    buttonDisable = hexToImVec4("#262626FF");
    buttonDisableBorder = buttonDisable;
    buttonDisableText = hexToImVec4("#FFFFFFFF");
    buttonDisableHovered = buttonDisable;
    buttonDisableHoveredText = buttonDisableText;
    buttonDisableActive = buttonDisable;
    buttonDisableActiveText = buttonDisableText;
    buttonDisableDisabled = buttonDisable;

    scrollbarBackground = hexToImVec4("#262626FF");
    scrollbarHandle = hexToImVec4("#B3B3B3FF");
    scrollbarHandleHovered = hexToImVec4("#B3B3B3FF");
    scrollbarHandleActive = hexToImVec4("#B3B3B3FF");

    sliderGrab = buttonPrimary;
    sliderGrabActive = buttonPrimaryActive;

    background = hexToImVec4("#1E1C1AFF");
    primary = buttonSecondary;
    secondary = buttonSecondary;
    disabled = hexToImVec4("#0F0F0FFF");
    invisible = hexToImVec4("#FFFFFF00");
    white = hexToImVec4("#FFFFFFFF");
    black = hexToImVec4("#000000FF");

    title = panelTitleBarActiveText;
    label = formLabel;
    input = formInputText;
    backgroundIcon = navbarIcon;

    // Load theme from JSON if provided
    if (!theme.empty()) {
        loadFromJSON(theme);
    }
}

bool Theme::loadFromJSON(const json& theme) {
    if (theme.contains("name") && theme["name"].is_string()) {
        name = theme["name"].get<std::string>();
    }

    auto setColor = [&](const char* key, ImVec4& target) -> bool {
        if (!theme.contains(key) || !theme[key].is_string()) {
            return false;
        }

        target = parseColorString(theme[key].get<std::string>(), target);
        return true;
    };

    setColor("panelBackground", panelBackground);
    setColor("panelBorder", panelBorder);
    setColor("panelBorderActive", panelBorderActive);
    setColor("panelText", panelText);
    setColor("panelTitleBar", panelTitleBar);
    setColor("panelTitleBarText", panelTitleBarText);
    setColor("panelTitleBarActive", panelTitleBarActive);
    setColor("panelTitleBarActiveText", panelTitleBarActiveText);
    setColor("panelFooter", panelFooter);
    setColor("panelFooterText", panelFooterText);

    setColor("text", text);
    setColor("textMuted", textMuted);
    setColor("textHighlighted", textHighlighted);
    setColor("positive", positive);
    setColor("negative", negative);
    const bool hasWarning = setColor("warning", warning);
    setColor("listItemBackground", listItemBackground);
    setColor("groupBackground", listItemBackground);

    setColor("tabsBackground", tabsBackground);
    setColor("tabsBorder", tabsBorder);
    setColor("tab", tab);
    setColor("tabText", tabText);
    setColor("tabHovered", tabHovered);
    setColor("tabActive", tabActive);
    setColor("tabActiveText", tabActiveText);

    setColor("navbarBackground", navbarBackground);
    setColor("navbarBorder", navbarBorder);
    setColor("navbarIcon", navbarIcon);
    setColor("navbarIconActive", navbarIconActive);

    setColor("formLabel", formLabel);
    setColor("formInputBackground", formInputBackground);
    setColor("formInputBorder", formInputBorder);
    setColor("formInputBorderActive", formInputBorderActive);
    setColor("formInputText", formInputText);
    setColor("formInputTextActive", formInputTextActive);
    setColor("formHelpText", formHelpText);

    setColor("buttonPrimary", buttonPrimary);
    const bool hasButtonPrimaryBorder = setColor("buttonPrimaryBorder", buttonPrimaryBorder);
    setColor("buttonPrimaryText", buttonPrimaryText);
    setColor("buttonPrimaryHovered", buttonPrimaryHovered);
    const bool hasButtonPrimaryHoveredText = setColor("buttonPrimaryHoveredText", buttonPrimaryHoveredText);
    setColor("buttonPrimaryActive", buttonPrimaryActive);
    const bool hasButtonPrimaryActiveText = setColor("buttonPrimaryActiveText", buttonPrimaryActiveText);
    setColor("buttonPrimaryDisabled", buttonPrimaryDisabled);

    setColor("buttonSecondary", buttonSecondary);
    const bool hasButtonSecondaryBorder = setColor("buttonSecondaryBorder", buttonSecondaryBorder);
    setColor("buttonSecondaryText", buttonSecondaryText);
    setColor("buttonSecondaryHovered", buttonSecondaryHovered);
    const bool hasButtonSecondaryHoveredText = setColor("buttonSecondaryHoveredText", buttonSecondaryHoveredText);
    setColor("buttonSecondaryActive", buttonSecondaryActive);
    const bool hasButtonSecondaryActiveText = setColor("buttonSecondaryActiveText", buttonSecondaryActiveText);
    setColor("buttonSecondaryDisabled", buttonSecondaryDisabled);

    setColor("buttonEnable", buttonEnable);
    const bool hasButtonEnableBorder = setColor("buttonEnableBorder", buttonEnableBorder);
    const bool hasButtonEnableText = setColor("buttonEnableText", buttonEnableText);
    const bool hasButtonEnableHovered = setColor("buttonEnableHovered", buttonEnableHovered);
    const bool hasButtonEnableHoveredText = setColor("buttonEnableHoveredText", buttonEnableHoveredText);
    const bool hasButtonEnableActive = setColor("buttonEnableActive", buttonEnableActive);
    const bool hasButtonEnableActiveText = setColor("buttonEnableActiveText", buttonEnableActiveText);
    const bool hasButtonEnableDisabled = setColor("buttonEnableDisabled", buttonEnableDisabled);

    setColor("buttonDisable", buttonDisable);
    const bool hasButtonDisableBorder = setColor("buttonDisableBorder", buttonDisableBorder);
    const bool hasButtonDisableText = setColor("buttonDisableText", buttonDisableText);
    const bool hasButtonDisableHovered = setColor("buttonDisableHovered", buttonDisableHovered);
    const bool hasButtonDisableHoveredText = setColor("buttonDisableHoveredText", buttonDisableHoveredText);
    const bool hasButtonDisableActive = setColor("buttonDisableActive", buttonDisableActive);
    const bool hasButtonDisableActiveText = setColor("buttonDisableActiveText", buttonDisableActiveText);
    const bool hasButtonDisableDisabled = setColor("buttonDisableDisabled", buttonDisableDisabled);

    setColor("scrollbarBackground", scrollbarBackground);
    setColor("scrollbarHandle", scrollbarHandle);
    setColor("scrollbarHandleHovered", scrollbarHandleHovered);
    setColor("scrollbarHandleActive", scrollbarHandleActive);

    const bool hasSliderGrab = setColor("sliderGrab", sliderGrab);
    const bool hasSliderGrabActive = setColor("sliderGrabActive", sliderGrabActive);

    setColor("background", background);
    setColor("primary", primary);
    setColor("secondary", secondary);
    setColor("disabled", disabled);
    setColor("invisible", invisible);
    setColor("white", white);
    setColor("black", black);
    const bool hasTitle = setColor("title", title);
    const bool hasLabel = setColor("label", label);
    const bool hasInput = setColor("input", input);
    const bool hasBackgroundIcon = setColor("backgroundIcon", backgroundIcon);

    if (!hasButtonPrimaryBorder) {
        buttonPrimaryBorder = buttonPrimary;
    }
    if (!hasButtonPrimaryHoveredText) {
        buttonPrimaryHoveredText = buttonPrimaryText;
    }
    if (!hasButtonPrimaryActiveText) {
        buttonPrimaryActiveText = buttonPrimaryText;
    }

    if (!hasButtonSecondaryBorder) {
        buttonSecondaryBorder = buttonSecondary;
    }
    if (!hasButtonSecondaryHoveredText) {
        buttonSecondaryHoveredText = buttonSecondaryText;
    }
    if (!hasButtonSecondaryActiveText) {
        buttonSecondaryActiveText = buttonSecondaryText;
    }

    if (!hasButtonEnableBorder) {
        buttonEnableBorder = buttonEnable;
    }
    if (!hasButtonEnableText) {
        buttonEnableText = buttonPrimaryText;
    }
    if (!hasButtonEnableHovered) {
        buttonEnableHovered = buttonEnable;
    }
    if (!hasButtonEnableHoveredText) {
        buttonEnableHoveredText = buttonEnableText;
    }
    if (!hasButtonEnableActive) {
        buttonEnableActive = buttonEnable;
    }
    if (!hasButtonEnableActiveText) {
        buttonEnableActiveText = buttonEnableText;
    }
    if (!hasButtonEnableDisabled) {
        buttonEnableDisabled = buttonPrimaryDisabled;
    }

    if (!hasButtonDisableBorder) {
        buttonDisableBorder = buttonDisable;
    }
    if (!hasButtonDisableText) {
        buttonDisableText = white;
    }
    if (!hasButtonDisableHovered) {
        buttonDisableHovered = buttonDisable;
    }
    if (!hasButtonDisableHoveredText) {
        buttonDisableHoveredText = buttonDisableText;
    }
    if (!hasButtonDisableActive) {
        buttonDisableActive = buttonDisable;
    }
    if (!hasButtonDisableActiveText) {
        buttonDisableActiveText = buttonDisableText;
    }
    if (!hasButtonDisableDisabled) {
        buttonDisableDisabled = buttonDisable;
    }

    if (!hasSliderGrab) {
        sliderGrab = buttonPrimary;
    }
    if (!hasSliderGrabActive) {
        sliderGrabActive = buttonPrimaryActive;
    }

    if (!hasWarning) {
        warning = textHighlighted;
    }
    if (!hasTitle) {
        title = panelTitleBarActiveText;
    }
    if (!hasLabel) {
        label = formLabel;
    }
    if (!hasInput) {
        input = formInputText;
    }
    if (!hasBackgroundIcon) {
        backgroundIcon = navbarIcon;
    }

    return true;
}

void Theme::apply() {
    ImGuiStyle* style = &ImGui::GetStyle();

    // Layout
    style->WindowBorderSize = 1;
    style->PopupBorderSize = 1;
    style->WindowPadding = ImVec2(20, 20);
    style->ItemSpacing = ImVec2(5, 5);
    style->ScrollbarSize = 20;
    style->GrabMinSize = 20;

    style->WindowRounding = 8;
    style->FrameRounding = 8;
    style->PopupRounding = 8;
    style->ScrollbarRounding = 8;
    style->GrabRounding = 2;
    style->WindowTitleAlign = ImVec2(0.5f, 0.5f);

    style->DisplaySafeAreaPadding = ImVec2(20, 20);

    ImVec4* colors = ImGui::GetStyle().Colors;
    colors[ImGuiCol_Text] = panelText;
    colors[ImGuiCol_TitleBg] = panelTitleBar;
    colors[ImGuiCol_TitleBgCollapsed] = panelTitleBar;
    colors[ImGuiCol_TitleBgActive] = panelTitleBarActive;
    colors[ImGuiCol_WindowBg] = panelBackground;
    colors[ImGuiCol_Border] = panelBorder;
    colors[ImGuiCol_FrameBg] = formInputBackground;
    colors[ImGuiCol_FrameBgHovered] = formInputBackground;
    colors[ImGuiCol_FrameBgActive] = formInputBackground;
    colors[ImGuiCol_Button] = buttonSecondary;
    colors[ImGuiCol_ButtonHovered] = buttonSecondaryHovered;
    colors[ImGuiCol_ButtonActive] = buttonSecondaryActive;
    colors[ImGuiCol_ScrollbarBg] = scrollbarBackground;
    colors[ImGuiCol_ScrollbarGrab] = scrollbarHandle;
    colors[ImGuiCol_ScrollbarGrabHovered] = scrollbarHandleHovered;
    colors[ImGuiCol_ScrollbarGrabActive] = scrollbarHandleActive;
    colors[ImGuiCol_SliderGrab] = sliderGrab;
    colors[ImGuiCol_SliderGrabActive] = sliderGrabActive;
    colors[ImGuiCol_ResizeGrip] = buttonSecondary;
    colors[ImGuiCol_ResizeGripHovered] = buttonSecondaryHovered;
    colors[ImGuiCol_ResizeGripActive] = buttonSecondaryHovered;
    colors[ImGuiCol_CheckMark] = panelText;

    colors[ImGuiCol_Tab] = tab;
    colors[ImGuiCol_TabHovered] = tabHovered;
    colors[ImGuiCol_TabActive] = tabActive;
    colors[ImGuiCol_TabUnfocused] = tab;
    colors[ImGuiCol_TabUnfocusedActive] = tabActive;

    colors[ImGuiCol_DragDropTarget] = ImVec4(0.3f, 0.3f, 0.3f, 0.4f);
}

// Function to convert a color hex string to ImVec4
ImVec4 Theme::hexToImVec4(const std::string& hex) {
    std::string value = trim(hex);
    if (!value.empty() && value[0] == '#') {
        value = value.substr(1);
    }

    if (value.size() == 6) {
        value += "FF";
    }

    if (value.size() != 8) {
        return ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    }

    try {
        const auto parse = [&](size_t offset) -> float {
            return static_cast<float>(std::stoul(value.substr(offset, 2), nullptr, 16)) / 255.0f;
        };

        return ImVec4(parse(0), parse(2), parse(4), parse(6));
    }
    catch (const std::exception&) {
        return ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    }
}

// Function to convert a rgba() value to ImVec4
ImVec4 Theme::rgbaToImVec4(const std::string& rgba) {
    std::stringstream ss(rgba);
    std::string token;
    std::vector<std::string> parts;

    while (std::getline(ss, token, ',')) {
        parts.push_back(trim(token));
    }

    if (parts.size() < 3) {
        return ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    }

    try {
        float r = std::stof(parts[0]);
        float g = std::stof(parts[1]);
        float b = std::stof(parts[2]);
        float a = 255.0f;

        if (parts.size() >= 4) {
            a = std::stof(parts[3]);
            if (a <= 1.0f) {
                a *= 255.0f;
            }
        }

        r = std::clamp(r, 0.0f, 255.0f);
        g = std::clamp(g, 0.0f, 255.0f);
        b = std::clamp(b, 0.0f, 255.0f);
        a = std::clamp(a, 0.0f, 255.0f);

        return ImVec4(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f);
    }
    catch (const std::exception&) {
        return ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    }
}

// Function to parse a string to see if it's a hex or rgba() value
ImVec4 Theme::parseColorString(const std::string& colorString, ImVec4 defaultColor) {
    const std::string value = trim(colorString);
    if (value.empty()) {
        return defaultColor;
    }

    if (value[0] == '#') {
        return hexToImVec4(value);
    }

    if (value.rfind("rgba(", 0) == 0 && value.back() == ')') {
        return rgbaToImVec4(value.substr(5, value.size() - 6));
    }

    if (value.rfind("rgb(", 0) == 0 && value.back() == ')') {
        return rgbaToImVec4(value.substr(4, value.size() - 5));
    }

    return defaultColor;
}
