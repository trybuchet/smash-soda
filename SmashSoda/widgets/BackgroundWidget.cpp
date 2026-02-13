#include "BackgroundWidget.h"

BackgroundWidget::BackgroundWidget() {
}

void BackgroundWidget::render() {
    ImGuiViewport* viewport = ImGui::GetMainViewport();  // Get the main viewport

    ImGui::SetNextWindowPos(viewport->Pos);  // Position at the top-left
    ImGui::SetNextWindowSize(viewport->Size); // Make it fullscreen

    // Ensure background doesn't take focus or block interaction
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ThemeController::getInstance().getActiveTheme()->background);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

    ImGui::SetNextWindowBgAlpha(1.0f);

    ImGui::Begin("##Background", nullptr,
        ImGuiWindowFlags_NoDecoration |
        ImGuiWindowFlags_NoBringToFrontOnFocus |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoInputs |
        ImGuiWindowFlags_NoNav |
        ImGuiWindowFlags_NoMouseInputs);

    ImGui::End();
    ImGui::PopStyleColor();
    ImGui::PopStyleVar();
}