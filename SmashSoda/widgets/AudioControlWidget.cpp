#include "AudioControlWidget.h"
#include "../globals/AppFonts.h"
#include "../services/ThemeController.h"

bool AudioControlWidget::render(
	const char* id, int* volume, bool isEnabled, float preview, Icon iconOn, Icon iconOff,
	AudioControlWidget::Action releaseVolumeCallback
)
{
	static bool wasClicked = false;
	wasClicked = false;

	static float width;
	width = ImGui::GetContentRegionAvail().x;

	Theme* theme = ThemeController::getInstance().getActiveTheme();
	ImGui::PushStyleColor(ImGuiCol_Text, theme->formInputText);
	ImGui::BeginChild(
		(string("###AudioControl") + string(id)).c_str(),
		ImVec2(width, 50.0f)
	);
	
	// Mute button
	if (ToggleIconButtonWidget::render(iconOn, iconOff, isEnabled, theme->positive, theme->negative, ImVec2(30, 30)))
	{
		wasClicked = true;
	}

	ImGui::SameLine();

	// Volume control
	ImGui::BeginGroup();
	AppFonts::pushTitle();
	ImGui::SetNextItemWidth(width - 64.0f);
	ImGui::PushStyleColor(ImGuiCol_FrameBg, theme->formInputBackground);
	ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, theme->formInputBackground);
	ImGui::PushStyleColor(ImGuiCol_FrameBgActive, theme->formInputBackground);
	ImGui::PushStyleColor(ImGuiCol_SliderGrab, theme->sliderGrab);
	ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, theme->sliderGrabActive);
	ImGui::SliderInt(
		(string("###AudioControlSlider") + string(id)).c_str(),
		volume, 0, 100, "%d%%"
	);
	ImGui::PopStyleColor(5);
	if (ImGui::IsItemHovered()) ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

	if (releaseVolumeCallback && ImGui::IsItemDeactivatedAfterEdit())
	{
		releaseVolumeCallback();
	}
	
	// Preview bar
	const float clampedPreview = preview < 0.0f ? 0.0f : (preview > 1.0f ? 1.0f : preview);
	ImGui::PushStyleColor(ImGuiCol_FrameBg, theme->formInputBackground);
	ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, theme->formInputBackground);
	ImGui::PushStyleColor(ImGuiCol_FrameBgActive, theme->formInputBackground);
	if (clampedPreview > 0.65f) ImGui::PushStyleColor(ImGuiCol_PlotHistogram, theme->negative);
	else if (clampedPreview > 0.45f) ImGui::PushStyleColor(ImGuiCol_PlotHistogram, theme->buttonPrimary);
	else ImGui::PushStyleColor(ImGuiCol_PlotHistogram, theme->positive);
	ImGui::ProgressBar(clampedPreview, ImVec2(width - 64.0f, 10.0f), "");
	ImGui::PopStyleColor(4);

	AppFonts::pop();
	ImGui::EndGroup();

	ImGui::EndChild();
	ImGui::PopStyleColor();

	return wasClicked;
}



