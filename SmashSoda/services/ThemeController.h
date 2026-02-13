#pragma once

#include <vector>
#include <memory>
#include <string>
#include "imgui/imgui.h"
#include "../models/Theme.h"
#include "../themes/AquaTheme.h"
#include "../themes/BubblegumTheme.h"
#include "../themes/EmberTheme.h"
#include "../themes/EmeraldTheme.h"
#include "../themes/IndigoTheme.h"
#include "../themes/GoldenTheme.h"
#include "../themes/WildfireTheme.h"
#include "helpers/PathHelper.h"

class ThemeController {
public:
    // Singleton
    static ThemeController& getInstance() {
        static ThemeController instance;
        return instance;
    }

    // Delete copy/move constructors to enforce singleton
    ThemeController(const ThemeController&) = delete;
    ThemeController& operator=(const ThemeController&) = delete;
    ThemeController(ThemeController&&) = delete;
    ThemeController& operator=(ThemeController&&) = delete;

    void addTheme(std::unique_ptr<Theme> theme);
    void applyTheme(const std::string& name);
    Theme* getActiveTheme();
    std::vector<std::unique_ptr<Theme>>& getThemes();
    std::vector<std::pair<std::string, std::string>> getThemeNames();

private:
    ThemeController();               // Private constructor
    void loadDefaultThemes();        // Load built-in themes
    void loadCustomThemes();         // Load custom themes

    std::vector<std::unique_ptr<Theme>> _themes;
    Theme* _activeTheme = nullptr;
};