#pragma once

#include "../models/Theme.h"

class IndigoTheme : public Theme {
public:

    IndigoTheme() : Theme("Indigo") {

        ImVec4 primary = hexToImVec4("#5d2196ff");

        panelBackground = hexToImVec4("#302E2BFF");
        panelBorder = hexToImVec4("#FFFFFF33");
        panelBorderActive = primary;
        panelText = hexToImVec4("#F2EEDDFF");
        panelTitleBar = hexToImVec4("#F2EEDDFF");
        panelTitleBarText = hexToImVec4("#302E2BFF");
        panelTitleBarActive = primary;
        panelTitleBarActiveText = hexToImVec4("#FFFFFFFF");
        panelFooter = hexToImVec4("#282623FF");
        panelFooterText = hexToImVec4("#B3B3B3FF");

        text = hexToImVec4("#F2EEDDFF");
        textMuted = hexToImVec4("#B3B3B3FF");
        textHighlighted = hexToImVec4("#aa40e8ff");
        positive = hexToImVec4("#8CBF8CFF");
        negative = hexToImVec4("#CC7373FF");

        listItemBackground = hexToImVec4("#353330ff");

        tabsBackground = hexToImVec4("#282623FF");
        tabsBorder = primary;
        tab = hexToImVec4("#F2EEDDFF");
        tabText = hexToImVec4("#302E2BFF");
        tabHovered = primary;
        tabActive = primary;
        tabActiveText = hexToImVec4("#FFFFFFFF");

        navbarBackground = hexToImVec4("#0F0F0FFF");
        navbarBorder = hexToImVec4("#262626FF");
        navbarIcon = hexToImVec4("#FFFFFF26");
        navbarIconActive = hexToImVec4("#aa40e8ff");

        formLabel = hexToImVec4("#c068f3ff");
        formInputBackground = hexToImVec4("#1A1A1AFF");
        formInputBorder = hexToImVec4("#262626FF");
        formInputBorderActive = hexToImVec4("#ffffffff");
        formInputText = hexToImVec4("#B3B3B3FF");
        formInputTextActive = hexToImVec4("#FFFFFFFF");
        formHelpText = hexToImVec4("#f1f1f1ff");

        buttonPrimary = primary;
        buttonPrimaryBorder = primary;
        buttonPrimaryText = hexToImVec4("#FFFFFFFF");
        buttonPrimaryHovered = primary;
        buttonPrimaryHoveredText = hexToImVec4("#FFFFFFFF");
        buttonPrimaryActive = primary;
        buttonPrimaryActiveText = hexToImVec4("#FFFFFFFF");
        buttonPrimaryDisabled = hexToImVec4("#262626FF");

        buttonSecondary = hexToImVec4("#F2EEDDFF");
        buttonSecondaryBorder = hexToImVec4("#00000033");
        buttonSecondaryText = hexToImVec4("#302E2BFF");
        buttonSecondaryHovered = hexToImVec4("#FAF3D9FF");
        buttonSecondaryHoveredText = hexToImVec4("#302E2BFF");
        buttonSecondaryActive = hexToImVec4("#d4d4d4ff");
        buttonSecondaryActiveText = hexToImVec4("#FFFFFFFF");
        buttonSecondaryDisabled = hexToImVec4("#262626FF");

        buttonEnable = primary;
        buttonDisable = hexToImVec4("#262626FF");

        scrollbarBackground = hexToImVec4("#262626FF");
        scrollbarHandle = hexToImVec4("#B3B3B3FF");
        scrollbarHandleHovered = hexToImVec4("#B3B3B3FF");
        scrollbarHandleActive = hexToImVec4("#B3B3B3FF");
        sliderGrab = buttonPrimary;
        sliderGrabActive = buttonPrimaryActive;

        background = hexToImVec4("#1E1C1AFF"); // Dark background
        primary = buttonSecondary;
        secondary = buttonSecondary;
        disabled = hexToImVec4("#0F0F0FFF"); // Muted / Disabled
        invisible = hexToImVec4("#FFFFFF00");
    }

    virtual void apply() {
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
};
