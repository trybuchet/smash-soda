#include "ConfirmPopupWidget.h"
#include "../services/ThemeController.h"
#include "../globals/AppFonts.h"

bool ConfirmPopupWidget::render(const char* title, bool& showPopup, std::string textString)
{
    static bool result;
    result = false;

    static ImVec2 res;
    static ImVec2 cursor;

    float uiScale = ThemeController::getInstance().getUiScale();
    if (uiScale <= 0.0f) {
        uiScale = 1.0f;
    }

    const ImVec2 windowSize(400.0f * uiScale, 180.0f * uiScale);
    const float contentPad = 10.0f * uiScale;
    const float buttonGap = 12.0f * uiScale;

    res = ImGui::GetMainViewport()->Size;

    ImGui::SetNextWindowSize(windowSize);
    ImGui::SetNextWindowPos(
        ImVec2(
            (res.x / 2) - (windowSize.x / 2),
            (res.y / 2) - (windowSize.y / 2)
        )
    );

    Theme* theme = ThemeController::getInstance().getActiveTheme();
    ImGui::PushFont(AppFonts::title);
    ImGui::PushStyleColor(ImGuiCol_Text, theme->panelText);

    if (ImGui::BeginPopupModal(
            title,
            &showPopup,
            ImGuiWindowFlags_NoSavedSettings |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoResize))
    {
        const ImGuiStyle& style = ImGui::GetStyle();

        if (textString.size() > 0)
        {
            cursor = ImGui::GetCursorPos();
            ImGui::SetCursorPos(ImVec2(cursor.x + contentPad, cursor.y + contentPad));

            ImGui::PushFont(AppFonts::input);
            ImGui::PushStyleColor(ImGuiCol_Text, theme->formInputText);
            ImGui::TextWrapped(textString.c_str());
            ImGui::PopStyleColor();
            ImGui::PopFont();
        }

        const float buttonPadX = 20.0f * uiScale;
        const float buttonPadY = 10.0f * uiScale;
        const float cancelWidth = ImGui::CalcTextSize("Cancel").x + buttonPadX;
        const float confirmWidth = ImGui::CalcTextSize("Confirm").x + buttonPadX;
        const float buttonsWidth = cancelWidth + buttonGap + confirmWidth;
        const float buttonHeight = ImGui::GetTextLineHeight() + buttonPadY;
        const ImVec2 windowPos = ImGui::GetWindowPos();
        const ImVec2 windowSizeNow = ImGui::GetWindowSize();

        const float buttonX = windowPos.x + windowSizeNow.x - style.WindowPadding.x - contentPad - buttonsWidth;
        const float buttonY = windowPos.y + windowSizeNow.y - style.WindowPadding.y - contentPad - buttonHeight;

        ImGui::SetCursorScreenPos(ImVec2(buttonX, buttonY));

        if (Widget::elBtnSecondary("Cancel"))
        {
            showPopup = false;
            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine(0.0f, buttonGap);

        if (Widget::elBtn("Confirm"))
        {
            result = true;
            showPopup = false;
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }

    ImGui::PopStyleColor();
    ImGui::PopFont();

    return result;
}
