#include "TooltipWidget.h"

bool TooltipWidget::render(const char* text, bool forceShow)
{
	if (ImGui::IsItemHovered() || forceShow)
	{
		Theme* theme = ThemeController::getInstance().getActiveTheme();
		ImGui::PushStyleColor(ImGuiCol_Text, theme->text);

		ImGui::SetTooltip("%s", text != nullptr ? text : "");

		ImGui::PopStyleColor();
		return true;
	}

	return false;
}



