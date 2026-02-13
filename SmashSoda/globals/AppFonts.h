#pragma once

#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_win32.h"
#include "../imgui/imgui_impl_dx11.h"

#define APPFONTS_EXTRA_LIGHT	"Fonts/AfacadFlux-ExtraLight.ttf"
#define APPFONTS_LIGHT			"Fonts/AfacadFlux-Light.ttf"
#define APPFONTS_REGULAR		"Fonts/AfacadFlux-Regular.ttf"
#define APPFONTS_MEDIUM			"Fonts/AfacadFlux-Medium.ttf"
#define APPFONTS_SEMI_BOLD		"Fonts/AfacadFlux-SemiBold.ttf"
#define APPFONTS_BOLD			"Fonts/AfacadFlux-Bold.ttf"
#define APPFONTS_EXTRA_BOLD		"Fonts/AfacadFlux-ExtraBold.ttf"

class AppFonts
{

public:
	static void init(ImGuiIO& io);

	static void pushLabel();
	static void pushTitle();
	static void pushInput();
	static void pushLarge();
	static void pushInputArrow();
	static void pushSugoiDekai();
	static void pop();

	static ImFont* label;
	static ImFont* title;
	static ImFont* input;
	static ImFont* large;
	static ImFont* inputArrow;
	static ImFont* sugoidekai;
};

