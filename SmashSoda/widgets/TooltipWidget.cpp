#include "TooltipWidget.h"

bool TooltipWidget::render(const char* text, bool forceShow)
{
	if (ImGui::IsItemHovered() || forceShow)
	{
		Theme* theme = ThemeController::getInstance().getActiveTheme();
		ImGui::PushStyleColor(ImGuiCol_Text, theme->text);

		ImGui::SetTooltip(text);
		return true;
	}
}



