#include "PopupWidgetEdit.h"
#include "../services/ThemeController.h"

bool PopupWidgetEdit::render(const char* title, bool& showPopup, std::string &oldReason)
{
    static bool result;
    result = false;
    static char newReason[32] = "";
    static ImVec2 BUTTON_SIZE = ImVec2(64, 64);
    bool pressedEnter = false;

    static ImVec2 res;
    res = ImGui::GetMainViewport()->Size;
    ImVec2 windowSize(300, 250);
    static ImVec2 popupPosition;
    popupPosition = ImGui::GetCursorScreenPos();
    if (popupPosition.x < 0) popupPosition.x = 5;
    else if (popupPosition.x > (res.x - windowSize.x)) popupPosition.x = res.x - windowSize.x - 5;
    if (popupPosition.y < 0) popupPosition.y = 5;
    else if (popupPosition.y > (res.y - windowSize.y)) popupPosition.y = res.y - windowSize.y - 5;
    ImGui::SetNextWindowPos(popupPosition);

    ImGui::SetNextWindowSize(windowSize);
    ImGui::PushStyleColor(ImGuiCol_Text, AppColors::panelTitleBarActiveText);
    if (ImGui::BeginPopupModal(title, nullptr, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize))
    {

        static ImVec2 size;
        size = ImGui::GetContentRegionAvail();

        ImGui::BeginChild(title, ImVec2(10, 10));
        ImGui::EndChild();
        ImGui::SetNextItemWidth(size.x);
        if (ImGui::InputText("##Reason", newReason, 32, ImGuiInputTextFlags_EnterReturnsTrue))
        {
            pressedEnter = true;
        }
        if (ImGui::IsWindowFocused() && !ImGui::IsAnyItemActive() && !ImGui::IsMouseClicked(0))
            ImGui::SetKeyboardFocusHere(-1);

        ImGui::Dummy(ImVec2(0.0f, 30.0f));

        ImGui::SetCursorPos(ImVec2(25, size.y - 50));
        if (IconButton::render(AppIcons::no, AppColors::negative, BUTTON_SIZE))
        {
            showPopup = false;
            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();
        ImGui::SetCursorPos(ImVec2(size.x - 77, size.y - 50));
        if (IconButton::render(AppIcons::yes, AppColors::positive, BUTTON_SIZE) || pressedEnter)
        {

            oldReason = newReason;

            result = true;
            showPopup = false;
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }

    ImGui::PopStyleColor(2);
    return result;
}



