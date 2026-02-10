#include "ToggleIconButtonWidget.h"

bool ToggleIconButtonWidget::render(Icon onIcon, Icon offIcon, bool isOn, ImVec4 tintOn, ImVec4 tintOff, ImVec2 size)
{
    if (isOn)
    {
        return IconButton::render(onIcon, tintOn, size);
    }
    else
    {
        return IconButton::render(offIcon, tintOff, size);
    }

    return false;
}

bool ToggleIconButtonWidget::render(Icon onIcon, Icon offIcon, bool isOn, ImVec2 size) {
    Theme* theme = ThemeController::getInstance().getActiveTheme();
    return ToggleIconButtonWidget::render(onIcon, offIcon, isOn, theme->navbarIconActive, theme->navbarIcon, size);
}