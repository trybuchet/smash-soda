#include "AppFonts.h"

ImFont* AppFonts::label;
ImFont* AppFonts::title;
ImFont* AppFonts::input;
ImFont* AppFonts::large;
ImFont* AppFonts::inputArrow;
ImFont* AppFonts::sugoidekai;

void AppFonts::init(ImGuiIO& io) {

	label = io.Fonts->AddFontFromFileTTF(APPFONTS_BOLD, 20.0f, nullptr, io.Fonts->GetGlyphRangesChineseFull());
	title = io.Fonts->AddFontFromFileTTF(APPFONTS_BOLD, 22.0f, nullptr, io.Fonts->GetGlyphRangesChineseFull());
	input = io.Fonts->AddFontFromFileTTF(APPFONTS_SEMI_BOLD, 18.0f, nullptr, io.Fonts->GetGlyphRangesChineseFull());
	large = io.Fonts->AddFontFromFileTTF(APPFONTS_SEMI_BOLD, 20.0f, nullptr, io.Fonts->GetGlyphRangesChineseFull());
	inputArrow = io.Fonts->AddFontFromFileTTF(APPFONTS_SEMI_BOLD, 1.0f);
	sugoidekai = io.Fonts->AddFontFromFileTTF(APPFONTS_BOLD, 20.0f);

}

void AppFonts::pushLabel()
{
	ImGui::PushFont(label);
}

void AppFonts::pushTitle()
{
	ImGui::PushFont(title);
}

void AppFonts::pushInput()
{
	ImGui::PushFont(input);
}

void AppFonts::pushLarge()
{
	ImGui::PushFont(large);
}

void AppFonts::pushInputArrow()
{
	ImGui::PushFont(inputArrow);
}

void AppFonts::pushSugoiDekai()
{
	ImGui::PushFont(sugoidekai);
}

void AppFonts::pop()
{
	ImGui::PopFont();
}
