#pragma once

#include "AppFonts.h"
#include "AppColors.h"

class AppStyle
{
public:
	static void pushTitle();
	static void pushLabel();
	static void pushInput();
	static void pushSmall();
	static void pushInputArrow();
	static void pushPositive();
	static void pushNegative();
	static void pushButton();
	static void popButton();
	static void pop();
};

