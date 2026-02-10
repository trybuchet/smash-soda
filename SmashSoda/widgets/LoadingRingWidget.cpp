#include "LoadingRingWidget.h"
#include "../services/ThemeController.h"

void LoadingRingWidget::render(bool resetAnimation)
{
	static ImDrawList* drawList = ImGui::GetWindowDrawList();

	ImVec2 gradient_size = ImVec2(ImGui::CalcItemWidth(), ImGui::GetFrameHeight());
	{
		static float speedSlow = 8.0f, speedFast = 16.0f;
		static float speedMin = speedSlow, speedMax = speedFast;
		static ImU32 color = ImGui::ColorConvertFloat4ToU32(AppColors::buttonSecondary);
		static float angle_min = 0, angle_max = 0;
		static ImVec2 p0, p1, center;

		if (resetAnimation) {
			angle_min = 0;
			angle_max = 0;
		}

		// Reduce the size of the widget
		float ringSize = 50.0f;
		float ringRadius = 12.0f;
		float ringThickness = 5.0f;

		p0 = ImGui::GetCursorScreenPos();
		p1 = ImVec2(p0.x + ringSize, p0.y + ringSize);
		center = mul(sum(p0, p1), 0.5f);
		color = ImGui::ColorConvertFloat4ToU32(AppColors::buttonSecondary);
		angle_min += (IM_PI / 360.0f) * speedMin;
		angle_max += (IM_PI / 360.0f) * speedMax;
		if (angle_max - angle_min > 2 * IM_PI)
		{
			speedMin = speedFast;
			speedMax = speedSlow;
		}
		else if (angle_min - angle_max > 2 * IM_PI)
		{
			speedMin = speedSlow;
			speedMax = speedFast;
		}

		// Draw the smaller ring
		drawList->PathArcTo(center, ringRadius, angle_min, angle_max, 32);
		drawList->PathStroke(color, 0, ringThickness);
	}
}