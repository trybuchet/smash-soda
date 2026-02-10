#include "AlertWidget.h"
#include "../services/ThemeController.h"
#include "../globals/AppFonts.h"

bool AlertWidget::render(const char * title, const char* message) {
    static bool result;
    result = false;

    static ImVec2 BUTTON_SIZE = ImVec2(64, 64);
    static ImVec2 res;
    static ImVec2 cursor;

    ImVec2 windowSize(400, 180);
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
        ImVec2 size = ImGui::GetContentRegionAvail();

        cursor = ImGui::GetCursorPos();
        ImGui::SetCursorPos(ImVec2(cursor.x + 10, cursor.y + 10));

        ImGui::PushFont(AppFonts::input);
        ImGui::PushStyleColor(ImGuiCol_Text, theme->formInputText);
        ImGui::TextWrapped(message);
        ImGui::PopStyleColor();
        ImGui::PopFont();

        ImGui::SetCursorPos(ImVec2(size.x - 54, size.y));

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



