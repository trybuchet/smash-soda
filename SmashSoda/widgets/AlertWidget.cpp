#include "AlertWidget.h"
#include "../services/ThemeController.h"
#include "../globals/AppFonts.h"

bool AlertWidget::render(const char * title, const char* message) {
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
            nullptr,
            ImGuiWindowFlags_NoSavedSettings |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoResize))
    {
        const ImVec2 contentMax = ImGui::GetWindowContentRegionMax();
        const ImGuiStyle& style = ImGui::GetStyle();

        cursor = ImGui::GetCursorPos();
        ImGui::SetCursorPos(ImVec2(cursor.x + contentPad, cursor.y + contentPad));

        ImGui::PushFont(AppFonts::input);
        ImGui::PushStyleColor(ImGuiCol_Text, theme->formInputText);
        ImGui::TextWrapped(message);
        ImGui::PopStyleColor();
        ImGui::PopFont();

        const float buttonWidth = ImGui::CalcTextSize("Okay").x + (style.FramePadding.x * 2.0f);
        const float buttonHeight = ImGui::GetFrameHeight();
        const float buttonX = contentMax.x - buttonWidth - contentPad;
        const float buttonY = contentMax.y - buttonHeight - contentPad;

        ImGui::SetCursorPos(ImVec2(buttonX, buttonY));

        if (Widget::elBtn("Okay")) {
            result = true;
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }

    ImGui::PopStyleColor();
    ImGui::PopFont();

    return result;
}



