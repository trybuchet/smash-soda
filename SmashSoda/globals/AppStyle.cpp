#include "AppStyle.h"

void AppStyle::pushTitle()
{
	AppFonts::pushTitle();
	AppColors::pushTitle();
}

void AppStyle::pushLabel()
{
	AppFonts::pushLabel();
	AppColors::pushLabel();
}

void AppStyle::pushInput()
{
	AppFonts::pushInput();
	AppColors::pushInput();
}

void AppStyle::pushSmall() 
{
	AppFonts::pushInput();
	AppColors::pushLabel();
}

void AppStyle::pushInputArrow()
{
	AppFonts::pushInputArrow();
	AppColors::pushInput();
}

void AppStyle::pushPositive()
{
	AppFonts::pushInput();
	AppColors::pushPositive();
}

void AppStyle::pushNegative()
{
	AppFonts::pushInput();
	AppColors::pushNegative();
}

void AppStyle::pushButton() {
	AppFonts::pushInput();
	AppColors::pushButton();
}

void AppStyle::popButton() {
	AppFonts::pop();
	AppColors::popButton();
}

void AppStyle::pop()
{
	AppFonts::pop();
	AppColors::pop();
}
