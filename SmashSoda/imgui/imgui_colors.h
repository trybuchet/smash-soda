#pragma once

#include "imgui.h"
#include "../globals/AppColors.h"

namespace ImGui {

	ImVec4 color_bg = ImVec4(0.08f, 0.08f, 0.08f, 1.00f);
	
	/**
	 * Parsec Soda Theme
	 * by FlavioFS
	 */
	void theme_soda() {

		// Layout
		ImGuiStyle* style = &ImGui::GetStyle();

		style->WindowBorderSize = 1;
		style->PopupBorderSize = 1;
		style->WindowPadding = ImVec2(20, 20);
		style->ItemSpacing = ImVec2(5, 5);
		style->ScrollbarSize = 20;
		style->GrabMinSize = 20;

		style->WindowRounding = 8;
		style->FrameRounding = 4;
		style->PopupRounding = 4;
		style->ScrollbarRounding = 4;
		style->GrabRounding = 2;
		style->WindowTitleAlign = ImVec2(0.5f, 0.5f);

		style->DisplaySafeAreaPadding = ImVec2(20, 20);

		// Colors
		ImVec4* colors = ImGui::GetStyle().Colors;
		colors[ImGuiCol_Text] = ImVec4(0.75f, 0.75f, 0.75f, 1.00f);
		colors[ImGuiCol_TextDisabled] = ImVec4(0.35f, 0.35f, 0.35f, 1.00f);
		colors[ImGuiCol_WindowBg] = ImVec4(0.11f, 0.11f, 0.11f, 1.00f);
		colors[ImGuiCol_ChildBg] = ImVec4(0.11f, 0.11f, 0.11f, 1.00f);
		colors[ImGuiCol_PopupBg] = ImVec4(0.11f, 0.11f, 0.11f, 0.90f);
		colors[ImGuiCol_Border] = ImVec4(0.18f, 0.17f, 0.16f, 1.00f);
		colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_FrameBg] = ImVec4(0.08f, 0.07f, 0.07f, 1.00f);
		colors[ImGuiCol_FrameBgHovered] = ImVec4(0.08f, 0.07f, 0.07f, 1.00f);
		colors[ImGuiCol_FrameBgActive] = ImVec4(0.08f, 0.07f, 0.07f, 1.00f);
		colors[ImGuiCol_TitleBg] = ImVec4(0.00f, 0.39f, 0.47f, 1.00f);
		colors[ImGuiCol_TitleBgActive] = AppColors::primary;
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.32f, 0.39f, 1.00f);
		colors[ImGuiCol_MenuBarBg] = ImVec4(0.08f, 0.08f, 0.07f, 1.00f);
		colors[ImGuiCol_ScrollbarBg] = ImVec4(0.08f, 0.08f, 0.07f, 1.00f);
		colors[ImGuiCol_ScrollbarGrab] = AppColors::secondary;
		colors[ImGuiCol_ScrollbarGrabHovered] = AppColors::primary;
		colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.12f, 0.59f, 0.92f, 1.00f);
		colors[ImGuiCol_CheckMark] = ImVec4(0.90f, 0.90f, 0.90f, 0.50f);
		colors[ImGuiCol_SliderGrab] = AppColors::primary;
		colors[ImGuiCol_SliderGrabActive] = ImVec4(0.00f, 0.39f, 0.47f, 1.00f);
		colors[ImGuiCol_Button] = ImVec4(1.00f, 1.00f, 1.00f, 0.00f);
		colors[ImGuiCol_ButtonHovered] = ImVec4(1.00f, 1.00f, 1.00f, 0.08f);
		colors[ImGuiCol_ButtonActive] = ImVec4(1.00f, 1.00f, 1.00f, 0.50f);
		colors[ImGuiCol_Header] = AppColors::secondary;
		colors[ImGuiCol_HeaderHovered] = AppColors::primary;
		colors[ImGuiCol_HeaderActive] = ImVec4(0.12f, 0.59f, 0.92f, 1.00f);
		colors[ImGuiCol_Separator] = ImVec4(0.32f, 0.32f, 0.32f, 1.00f);
		colors[ImGuiCol_SeparatorHovered] = ImVec4(0.60f, 0.60f, 0.70f, 1.00f);
		colors[ImGuiCol_SeparatorActive] = ImVec4(0.70f, 0.70f, 0.90f, 1.00f);
		colors[ImGuiCol_ResizeGrip] = ImVec4(1.00f, 1.00f, 1.00f, 0.10f);
		colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.78f, 0.82f, 1.00f, 0.60f);
		colors[ImGuiCol_ResizeGripActive] = ImVec4(0.78f, 0.82f, 1.00f, 0.90f);
		colors[ImGuiCol_Tab] = AppColors::secondary;
		colors[ImGuiCol_TabHovered] = AppColors::primary;
		colors[ImGuiCol_TabActive] = ImVec4(0.12f, 0.59f, 0.92f, 1.00f);
		colors[ImGuiCol_TabUnfocused] = ImVec4(0.00f, 0.39f, 0.47f, 1.00f);
		colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.05f, 0.49f, 0.58f, 1.00f);
		colors[ImGuiCol_PlotLines] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
		colors[ImGuiCol_PlotLinesHovered] = AppColors::primary;
		colors[ImGuiCol_PlotHistogram] = AppColors::primary;
		colors[ImGuiCol_PlotHistogramHovered] = AppColors::secondary;
		colors[ImGuiCol_TableHeaderBg] = ImVec4(0.27f, 0.27f, 0.38f, 1.00f);
		colors[ImGuiCol_TableBorderStrong] = ImVec4(0.31f, 0.31f, 0.45f, 1.00f);
		colors[ImGuiCol_TableBorderLight] = ImVec4(0.26f, 0.26f, 0.28f, 1.00f);
		colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.07f);
		colors[ImGuiCol_TextSelectedBg] = AppColors::secondary;
		colors[ImGuiCol_DragDropTarget] = AppColors::primary;
		colors[ImGuiCol_NavHighlight] = ImVec4(0.45f, 0.45f, 0.90f, 0.80f);
		colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
		colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
		colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.10f, 0.11f, 0.18f, 0.35f);
	}

	/**
	 * ParsecSodaV Theme
	 * by v6ooo
	 */
	void theme_v() {

		ImGuiStyle* style = &ImGui::GetStyle();

		// Layout
		style->WindowBorderSize = 1;
		style->PopupBorderSize = 1;
		style->WindowPadding = ImVec2(10, 10);
		style->ItemSpacing = ImVec2(5, 5);
		style->ScrollbarSize = 10;
		style->GrabMinSize = 20;

		style->WindowRounding = 0;
		style->FrameRounding = 4;
		style->PopupRounding = 4;
		style->ScrollbarRounding = 4;
		style->GrabRounding = 2;
		style->WindowTitleAlign = ImVec2(0.5f, 0.5f);

		style->DisplaySafeAreaPadding = ImVec2(20, 20);

		// Colors
		ImVec4* colors = ImGui::GetStyle().Colors;
		colors[ImGuiCol_Text] = ImVec4(0.75f, 0.75f, 0.75f, 1.00f);
		colors[ImGuiCol_TextDisabled] = ImVec4(0.35f, 0.35f, 0.35f, 1.00f);
		colors[ImGuiCol_WindowBg] = ImVec4(0.0f, 0.0f, 0.0f, 1.00f);
		colors[ImGuiCol_ChildBg] = ImVec4(0.0f, 0.0f, 0.0f, 1.00f);
		colors[ImGuiCol_PopupBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.90f);
		colors[ImGuiCol_Border] = ImVec4(0.1f, 0.1f, 0.1f, 1.00f);
		colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_FrameBg] = ImVec4(0.05f, 0.05f, 0.05f, 1.00f);
		colors[ImGuiCol_FrameBgHovered] = ImVec4(0.05f, 0.05f, 0.05f, 1.00f);
		colors[ImGuiCol_FrameBgActive] = ImVec4(0.05f, 0.05f, 0.05f, 1.00f);
		colors[ImGuiCol_TitleBg] = ImVec4(0.05f, 0.05f, 0.05f, 1.00f);
		colors[ImGuiCol_TitleBgActive] = ImVec4(0.2f, 0.2f, 0.2f, 1.00f);
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.1f, 0.1f, 0.1f, 1.00f);
		colors[ImGuiCol_MenuBarBg] = ImVec4(0.08f, 0.08f, 0.07f, 1.00f);
		colors[ImGuiCol_ScrollbarBg] = ImVec4(0.05f, 0.05f, 0.05f, 1.00f);
		colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.2f, 0.2f, 0.2f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.3f, 0.3f, 0.3f, 1.00f);
		colors[ImGuiCol_CheckMark] = ImVec4(0.90f, 0.90f, 0.90f, 0.50f);
		colors[ImGuiCol_SliderGrab] = ImVec4(0.2f, 0.2f, 0.2f, 1.00f);
		colors[ImGuiCol_SliderGrabActive] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
		colors[ImGuiCol_Button] = ImVec4(1.00f, 1.00f, 1.00f, 0.00f);
		colors[ImGuiCol_ButtonHovered] = ImVec4(1.00f, 1.00f, 1.00f, 0.08f);
		colors[ImGuiCol_ButtonActive] = ImVec4(1.00f, 1.00f, 1.00f, 0.50f);
		colors[ImGuiCol_Header] = AppColors::secondary;
		colors[ImGuiCol_HeaderHovered] = AppColors::primary;
		colors[ImGuiCol_HeaderActive] = ImVec4(0.12f, 0.59f, 0.92f, 1.00f);
		colors[ImGuiCol_Separator] = ImVec4(0.11f, 0.11f, 0.11f, 1.00f);
		colors[ImGuiCol_SeparatorHovered] = ImVec4(0.60f, 0.60f, 0.70f, 1.00f);
		colors[ImGuiCol_SeparatorActive] = ImVec4(0.70f, 0.70f, 0.90f, 1.00f);
		colors[ImGuiCol_ResizeGrip] = ImVec4(1.00f, 1.00f, 1.00f, 0.03f);
		colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.78f, 0.82f, 1.00f, 0.30f);
		colors[ImGuiCol_ResizeGripActive] = ImVec4(0.78f, 0.82f, 1.00f, 0.60f);
		colors[ImGuiCol_Tab] = ImVec4(0.2f, 0.2f, 0.2f, 1.00f);
		colors[ImGuiCol_TabHovered] = ImVec4(0.35f, 0.35f, 0.35f, 1.00f);
		colors[ImGuiCol_TabActive] = ImVec4(0.3f, 0.3f, 0.3f, 1.00f);
		colors[ImGuiCol_TabUnfocused] = ImVec4(0.00f, 0.39f, 0.47f, 1.00f);
		colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.05f, 0.49f, 0.58f, 1.00f);
		colors[ImGuiCol_PlotLines] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
		colors[ImGuiCol_PlotLinesHovered] = AppColors::primary;
		colors[ImGuiCol_PlotHistogram] = AppColors::primary;
		colors[ImGuiCol_PlotHistogramHovered] = AppColors::secondary;
		colors[ImGuiCol_TableHeaderBg] = ImVec4(0.27f, 0.27f, 0.38f, 1.00f);
		colors[ImGuiCol_TableBorderStrong] = ImVec4(0.31f, 0.31f, 0.45f, 1.00f);
		colors[ImGuiCol_TableBorderLight] = ImVec4(0.26f, 0.26f, 0.28f, 1.00f);
		colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.07f);
		colors[ImGuiCol_TextSelectedBg] = AppColors::secondary;
		colors[ImGuiCol_DragDropTarget] = AppColors::primary;
		colors[ImGuiCol_NavHighlight] = ImVec4(0.45f, 0.45f, 0.90f, 0.80f);
		colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
		colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
		colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.10f, 0.11f, 0.18f, 0.35f);
	}

	/**
	 * Midnight Theme
	 * by Mickey Dearman
	 */
	void theme_midnight() {

		ImGuiStyle *style = &ImGui::GetStyle();

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

		// Colors
		AppColors::title = ImVec4(0.00f, 0.00f, 0.00f, 1.0f);
		AppColors::label = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
		AppColors::input = ImVec4(0.70f, 0.70f, 0.70f, 1.0f);
		AppColors::positive = ImVec4(0.18f, 0.59f, 1.00f, 1.00f);
		AppColors::negative = ImVec4(0.83f, 0.82f, 0.84f, .2f);
		AppColors::warning = ImVec4(0.83f, 0.35f, 0.03f, 1.00f);

		AppColors::white = ImVec4(0.83f, 0.82f, 0.84f, 1.00f);
		AppColors::black = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
		AppColors::invisible = ImVec4(1.00f, 1.00f, 1.00f, 0.00f);

		AppColors::primary = ImVec4(0.12f, 0.28f, 0.53f, 1.00f);
		AppColors::secondary = ImVec4(0.49f, 0.35f, 0.82f, 1.00f);

		AppColors::backgroundIcon = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);

		AppColors::disabled = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);

		// Colors
		ImVec4* colors = ImGui::GetStyle().Colors;
		colors[ImGuiCol_Text] = ImVec4(0.75f, 0.75f, 0.75f, 1.00f);
		colors[ImGuiCol_TextDisabled] = ImVec4(0.35f, 0.35f, 0.35f, 1.00f);
		colors[ImGuiCol_WindowBg] = ImVec4(0.11f, 0.11f, 0.11f, 1.00f);
		colors[ImGuiCol_ChildBg] = ImVec4(0.11f, 0.11f, 0.11f, 1.00f);
		colors[ImGuiCol_PopupBg] = ImVec4(0.11f, 0.11f, 0.11f, 0.90f);
		colors[ImGuiCol_Border] = ImVec4(0.18f, 0.17f, 0.16f, 1.00f);
		colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_FrameBg] = ImVec4(0.08f, 0.07f, 0.07f, 1.00f);
		colors[ImGuiCol_FrameBgHovered] = ImVec4(0.08f, 0.07f, 0.07f, 1.00f);
		colors[ImGuiCol_FrameBgActive] = ImVec4(0.08f, 0.07f, 0.07f, 1.00f);
		colors[ImGuiCol_TitleBg] = ImVec4(0.83f, 0.82f, 0.84f, 1.00f);
		colors[ImGuiCol_TitleBgActive] = AppColors::positive;
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.32f, 0.39f, 1.00f);
		colors[ImGuiCol_MenuBarBg] = ImVec4(0.08f, 0.08f, 0.07f, 1.00f);
		colors[ImGuiCol_ScrollbarBg] = ImVec4(0.08f, 0.08f, 0.07f, 1.00f);
		colors[ImGuiCol_ScrollbarGrab] = AppColors::secondary;
		colors[ImGuiCol_ScrollbarGrabHovered] = AppColors::primary;
		colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.12f, 0.59f, 0.92f, 1.00f);
		colors[ImGuiCol_CheckMark] = ImVec4(0.90f, 0.90f, 0.90f, 0.50f);
		colors[ImGuiCol_SliderGrab] = AppColors::primary;
		colors[ImGuiCol_SliderGrabActive] = ImVec4(0.00f, 0.39f, 0.47f, 1.00f);
		colors[ImGuiCol_Button] = ImVec4(1.00f, 1.00f, 1.00f, 0.00f);
		colors[ImGuiCol_ButtonHovered] = ImVec4(1.00f, 1.00f, 1.00f, 0.08f);
		colors[ImGuiCol_ButtonActive] = ImVec4(1.00f, 1.00f, 1.00f, 0.50f);
		colors[ImGuiCol_Header] = AppColors::secondary;
		colors[ImGuiCol_HeaderHovered] = AppColors::primary;
		colors[ImGuiCol_HeaderActive] = ImVec4(0.12f, 0.59f, 0.92f, 1.00f);
		colors[ImGuiCol_Separator] = ImVec4(0.32f, 0.32f, 0.32f, 1.00f);
		colors[ImGuiCol_SeparatorHovered] = ImVec4(0.60f, 0.60f, 0.70f, 1.00f);
		colors[ImGuiCol_SeparatorActive] = ImVec4(0.70f, 0.70f, 0.90f, 1.00f);
		colors[ImGuiCol_ResizeGrip] = ImVec4(1.00f, 1.00f, 1.00f, 0.10f);
		colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.78f, 0.82f, 1.00f, 0.60f);
		colors[ImGuiCol_ResizeGripActive] = ImVec4(0.78f, 0.82f, 1.00f, 0.90f);
		colors[ImGuiCol_Tab] = AppColors::secondary;
		colors[ImGuiCol_TabHovered] = AppColors::positive;
		colors[ImGuiCol_TabActive] = ImVec4(0.12f, 0.59f, 0.92f, 1.00f);
		colors[ImGuiCol_TabUnfocused] = ImVec4(0.00f, 0.39f, 0.47f, 1.00f);
		colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.05f, 0.49f, 0.58f, 1.00f);
		colors[ImGuiCol_PlotLines] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
		colors[ImGuiCol_PlotLinesHovered] = AppColors::primary;
		colors[ImGuiCol_PlotHistogram] = AppColors::primary;
		colors[ImGuiCol_PlotHistogramHovered] = AppColors::secondary;
		colors[ImGuiCol_TableHeaderBg] = ImVec4(0.27f, 0.27f, 0.38f, 1.00f);
		colors[ImGuiCol_TableBorderStrong] = ImVec4(0.31f, 0.31f, 0.45f, 1.00f);
		colors[ImGuiCol_TableBorderLight] = ImVec4(0.26f, 0.26f, 0.28f, 1.00f);
		colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.07f);
		colors[ImGuiCol_TextSelectedBg] = AppColors::positive;
		colors[ImGuiCol_DragDropTarget] = AppColors::primary;
		colors[ImGuiCol_NavHighlight] = ImVec4(0.45f, 0.45f, 0.90f, 0.80f);
		colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
		colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
		colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.10f, 0.11f, 0.18f, 0.35f);
	}

	/**
	 * Soda Arcade Theme
	 * by Mickey Dearman
	 */
	void theme_arcade() {

		ImGuiStyle* style = &ImGui::GetStyle();

		// Layout
		style->WindowBorderSize = 1;
		style->PopupBorderSize = 1;
		style->WindowPadding = ImVec2(20, 20);
		style->ItemSpacing = ImVec2(5, 5);
		style->ScrollbarSize = 20;
		style->GrabMinSize = 20;

		style->WindowRounding = 2;
		style->FrameRounding = 4;
		style->PopupRounding = 4;
		style->ScrollbarRounding = 4;
		style->GrabRounding = 2;
		style->WindowTitleAlign = ImVec2(0.5f, 0.5f);

		style->DisplaySafeAreaPadding = ImVec2(20, 20);

		// Colors
		ImVec4 purple1 = ImVec4(0.2f, 0.18f, 0.33f, 1.00f);
		ImVec4 purple2 = ImVec4(0.16f, 0.16f, 0.28f, 1.00f);
		ImVec4 purple3 = ImVec4(0.77f, 0.28f, 0.66f, 1.00f);

		ImVec4 pink1 = ImVec4(0.92f, 0.42f, 0.65f, 1.00f);
		ImVec4 pink2 = ImVec4(0.77f, 0.28f, 0.50f, 1.00f);
		ImVec4 cream = ImVec4(0.94f, 0.65f, 0.62f, 1.00f);

		color_bg = ImVec4(0.14f, 0.15f, 0.23f, 1.00f);

		AppColors::title = ImVec4(1.00f, 1.00f, 1.00f, 1.0f);
		AppColors::label = ImVec4(1.00f, 1.00f, 1.00f, .95f);
		AppColors::input = ImVec4(1.00f, 1.00f, 1.00f, 1.0f);
		AppColors::positive = pink1;
		AppColors::negative = ImVec4(0.83f, 0.82f, 0.84f, .2f);
		AppColors::warning = ImVec4(0.83f, 0.35f, 0.03f, 1.00f);

		AppColors::white = ImVec4(1,1,1, 1.00f);
		AppColors::black = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
		AppColors::invisible = ImVec4(1.00f, 1.00f, 1.00f, 0.00f);

		AppColors::primary = cream;
		AppColors::secondary = ImVec4(0.49f, 0.35f, 0.82f, 1.00f);

		AppColors::backgroundIcon = ImVec4(1.00f, 1.00f, 1.00f, .2f);

		AppColors::disabled = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);

		// Colors
		ImVec4* colors = ImGui::GetStyle().Colors;
		colors[ImGuiCol_Text] = ImVec4(1, 1, 1, 1.00f);
		colors[ImGuiCol_TextDisabled] = ImVec4(0.35f, 0.35f, 0.35f, 1.00f);
		colors[ImGuiCol_WindowBg] = purple1;
		colors[ImGuiCol_ChildBg] = purple1;
		colors[ImGuiCol_PopupBg] = purple1;
		colors[ImGuiCol_Border] = ImVec4(1.00f, 1.00f, 1.00f, 0.2f);
		colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_FrameBg] = color_bg;
		colors[ImGuiCol_FrameBgHovered] = ImVec4(0.08f, 0.07f, 0.07f, 1.00f);
		colors[ImGuiCol_FrameBgActive] = ImVec4(0.08f, 0.07f, 0.07f, 1.00f);
		colors[ImGuiCol_TitleBg] = purple2;
		colors[ImGuiCol_TitleBgActive] = pink2;
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.32f, 0.39f, 1.00f);
		colors[ImGuiCol_MenuBarBg] = ImVec4(0.08f, 0.08f, 0.07f, 1.00f);
		colors[ImGuiCol_ScrollbarBg] = ImVec4(0.08f, 0.07f, 0.07f, .3f);
		colors[ImGuiCol_ScrollbarGrab] = AppColors::secondary;
		colors[ImGuiCol_ScrollbarGrabHovered] = AppColors::primary;
		colors[ImGuiCol_ScrollbarGrabActive] = pink1;
		colors[ImGuiCol_CheckMark] = ImVec4(0.90f, 0.90f, 0.90f, 0.50f);
		colors[ImGuiCol_SliderGrab] = purple3;
		colors[ImGuiCol_SliderGrabActive] = ImVec4(0.00f, 0.39f, 0.47f, 1.00f);
		colors[ImGuiCol_Button] = ImVec4(1.00f, 1.00f, 1.00f, 0.00f);
		colors[ImGuiCol_ButtonHovered] = ImVec4(1.00f, 1.00f, 1.00f, 0.08f);
		colors[ImGuiCol_ButtonActive] = ImVec4(1.00f, 1.00f, 1.00f, 0.50f);
		colors[ImGuiCol_Header] = AppColors::secondary;
		colors[ImGuiCol_HeaderHovered] = pink2;
		colors[ImGuiCol_HeaderActive] = ImVec4(0.12f, 0.59f, 0.92f, 1.00f);
		colors[ImGuiCol_Separator] = pink2;
		colors[ImGuiCol_SeparatorHovered] = ImVec4(0.60f, 0.60f, 0.70f, 1.00f);
		colors[ImGuiCol_SeparatorActive] = ImVec4(0.70f, 0.70f, 0.90f, 1.00f);
		colors[ImGuiCol_ResizeGrip] = ImVec4(1.00f, 1.00f, 1.00f, 0.10f);
		colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.78f, 0.82f, 1.00f, 0.60f);
		colors[ImGuiCol_ResizeGripActive] = ImVec4(0.78f, 0.82f, 1.00f, 0.90f);
		colors[ImGuiCol_Tab] = purple2;
		colors[ImGuiCol_TabHovered] = ImVec4(0.77f, 0.28f, 0.66f, 1.00f);
		colors[ImGuiCol_TabActive] = pink2;
		colors[ImGuiCol_TabUnfocused] = ImVec4(0.00f, 0.39f, 0.47f, 1.00f);
		colors[ImGuiCol_TabUnfocusedActive] = pink2;
		colors[ImGuiCol_PlotLines] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
		colors[ImGuiCol_PlotLinesHovered] = pink2;
		colors[ImGuiCol_PlotHistogram] = pink2;
		colors[ImGuiCol_PlotHistogramHovered] = pink2;
		colors[ImGuiCol_TableHeaderBg] = ImVec4(0.27f, 0.27f, 0.38f, 1.00f);
		colors[ImGuiCol_TableBorderStrong] = ImVec4(0.31f, 0.31f, 0.45f, 1.00f);
		colors[ImGuiCol_TableBorderLight] = ImVec4(0.26f, 0.26f, 0.28f, 1.00f);
		colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.07f);
		colors[ImGuiCol_TextSelectedBg] = pink2;
		colors[ImGuiCol_DragDropTarget] = AppColors::primary;
		colors[ImGuiCol_NavHighlight] = ImVec4(0.45f, 0.45f, 0.90f, 0.80f);
		colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
		colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
		colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.10f, 0.11f, 0.18f, 0.35f);
	}

	/**
	 * Mini 
	 * by R3DPanda
	 */
	void theme_mini() {

		// Layout
		ImGuiStyle* style = &ImGui::GetStyle();

		style->WindowBorderSize = 1;
		style->PopupBorderSize = 1;
		style->WindowPadding = ImVec2(10, 10);
		style->ItemSpacing = ImVec2(5, 5);
		style->ScrollbarSize = 10;
		style->GrabMinSize = 20;

		style->WindowRounding = 0;
		style->FrameRounding = 4;
		style->PopupRounding = 4;
		style->ScrollbarRounding = 4;
		style->GrabRounding = 2;
		style->WindowTitleAlign = ImVec2(0.5f, 0.5f);

		style->DisplaySafeAreaPadding = ImVec2(20, 20);

		// Colors
		AppColors::primary = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
		AppColors::secondary = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);
		AppColors::positive = ImVec4(0.19f, 0.84f, 0.09f, 1.00f);
		AppColors::negative = ImVec4(0.80f, 0.00f, 0.10f, 1.00f);

		ImVec4* colors = ImGui::GetStyle().Colors;
		colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
		colors[ImGuiCol_TextDisabled] = ImVec4(0.35f, 0.35f, 0.35f, 1.00f);
		colors[ImGuiCol_WindowBg] = ImVec4(0.0f, 0.0f, 0.0f, 1.00f);
		colors[ImGuiCol_ChildBg] = ImVec4(0.0f, 0.0f, 0.0f, 1.00f);
		colors[ImGuiCol_PopupBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.90f);
		colors[ImGuiCol_Border] = ImVec4(0.1f, 0.1f, 0.1f, 1.00f);
		colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_FrameBg] = ImVec4(0.05f, 0.05f, 0.05f, 1.00f);
		colors[ImGuiCol_FrameBgHovered] = ImVec4(0.05f, 0.05f, 0.05f, 1.00f);
		colors[ImGuiCol_FrameBgActive] = ImVec4(0.05f, 0.05f, 0.05f, 1.00f);
		colors[ImGuiCol_TitleBg] = ImVec4(0.05f, 0.05f, 0.05f, 1.00f);
		colors[ImGuiCol_TitleBgActive] = ImVec4(0.2f, 0.2f, 0.2f, 1.00f);
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.1f, 0.1f, 0.1f, 1.00f);
		colors[ImGuiCol_MenuBarBg] = ImVec4(0.08f, 0.08f, 0.07f, 1.00f);
		colors[ImGuiCol_ScrollbarBg] = ImVec4(0.05f, 0.05f, 0.05f, 1.00f);
		colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.2f, 0.2f, 0.2f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.3f, 0.3f, 0.3f, 1.00f);
		colors[ImGuiCol_CheckMark] = ImVec4(0.90f, 0.90f, 0.90f, 0.50f);
		colors[ImGuiCol_SliderGrab] = ImVec4(0.2f, 0.2f, 0.2f, 1.00f);
		colors[ImGuiCol_SliderGrabActive] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
		colors[ImGuiCol_Button] = ImVec4(1.00f, 1.00f, 1.00f, 0.00f);
		colors[ImGuiCol_ButtonHovered] = ImVec4(1.00f, 1.00f, 1.00f, 0.08f);
		colors[ImGuiCol_ButtonActive] = ImVec4(1.00f, 1.00f, 1.00f, 0.50f);
		colors[ImGuiCol_Header] = AppColors::secondary;
		colors[ImGuiCol_HeaderHovered] = AppColors::primary;
		colors[ImGuiCol_HeaderActive] = ImVec4(0.40f, 0.40f, 0.40f, 1.00f); //0.12f, 0.59f, 0.92f
		colors[ImGuiCol_Separator] = ImVec4(0.11f, 0.11f, 0.11f, 1.00f);
		colors[ImGuiCol_SeparatorHovered] = ImVec4(0.60f, 0.60f, 0.70f, 1.00f);
		colors[ImGuiCol_SeparatorActive] = ImVec4(0.70f, 0.70f, 0.90f, 1.00f);
		colors[ImGuiCol_ResizeGrip] = ImVec4(1.00f, 1.00f, 1.00f, 0.03f);
		colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.78f, 0.82f, 1.00f, 0.30f);
		colors[ImGuiCol_ResizeGripActive] = ImVec4(0.78f, 0.82f, 1.00f, 0.60f);
		colors[ImGuiCol_Tab] = ImVec4(0.2f, 0.2f, 0.2f, 1.00f);
		colors[ImGuiCol_TabHovered] = ImVec4(0.35f, 0.35f, 0.35f, 1.00f);
		colors[ImGuiCol_TabActive] = ImVec4(0.3f, 0.3f, 0.3f, 1.00f);
		colors[ImGuiCol_TabUnfocused] = ImVec4(0.00f, 0.39f, 0.47f, 1.00f);
		colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.05f, 0.49f, 0.58f, 1.00f);
		colors[ImGuiCol_PlotLines] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
		colors[ImGuiCol_PlotLinesHovered] = AppColors::primary;
		colors[ImGuiCol_PlotHistogram] = AppColors::primary;
		colors[ImGuiCol_PlotHistogramHovered] = AppColors::secondary;
		colors[ImGuiCol_TableHeaderBg] = ImVec4(0.27f, 0.27f, 0.38f, 1.00f);
		colors[ImGuiCol_TableBorderStrong] = ImVec4(0.31f, 0.31f, 0.45f, 1.00f);
		colors[ImGuiCol_TableBorderLight] = ImVec4(0.26f, 0.26f, 0.28f, 1.00f);
		colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.07f);
		colors[ImGuiCol_TextSelectedBg] = AppColors::secondary;
		colors[ImGuiCol_DragDropTarget] = AppColors::primary;
		colors[ImGuiCol_NavHighlight] = ImVec4(0.45f, 0.45f, 0.90f, 0.80f);
		colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
		colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
		colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.10f, 0.11f, 0.18f, 0.35f);

	}

	ImVec4 loadStyle(unsigned int themeIndex) {

		switch (themeIndex) {
		case 1:
			theme_soda();
			break;
		case 2:
			theme_v();
			break;
		case 3:
			theme_mini();
			break;
		case 4:
			theme_arcade();
			break;
		default:
			theme_midnight();
			break;
		}

		return color_bg;

	}
}


