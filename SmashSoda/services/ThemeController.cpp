#include "ThemeController.h"
#include <imgui/imgui.h>

ThemeController::ThemeController() {
    loadDefaultThemes();
    loadCustomThemes();
    // Defer applying theme until ImGui context exists
    applyTheme("Ember"); // default theme (will no-op if no context yet)
}

void ThemeController::loadDefaultThemes() {
    addTheme(std::make_unique<AquaTheme>());
    addTheme(std::make_unique<BubblegumTheme>());
    addTheme(std::make_unique<EmberTheme>());
    addTheme(std::make_unique<EmeraldTheme>());
    addTheme(std::make_unique<GoldenTheme>());
    addTheme(std::make_unique<IndigoTheme>());
}

void ThemeController::loadCustomThemes() {
    const string themesPath = PathHelper::GetCurrentPath() + "\\themes\\";

    // Get all files in the directory
	const MTY_FileList* files = MTY_GetFileList(themesPath.c_str(), NULL);
    if (files->len > 0) {
        // foreach file in files->files
        for (int i = 0; i < files->len; i++) {

            // Get file
            const MTY_FileDesc* file = &files->files[i];

        }
    }
}

void ThemeController::addTheme(std::unique_ptr<Theme> theme) {
    _themes.push_back(std::move(theme));
}

void ThemeController::applyTheme(const std::string& name) {
    for (auto& theme : _themes) {
        if (theme->name == name) {
            _activeTheme = theme.get();
            // Only apply if ImGui context is ready
            if (ImGui::GetCurrentContext() != nullptr) {
                _activeTheme->apply();
            }
            break;
        }
    }
}

Theme* ThemeController::getActiveTheme() {
    return _activeTheme;
}

std::vector<std::unique_ptr<Theme>>& ThemeController::getThemes() {
    return _themes;
}

std::vector<std::pair<std::string, std::string>> ThemeController::getThemeNames() {
    std::vector<std::pair<std::string, std::string>> themePairs;
    for (const auto& theme : _themes) {
        themePairs.push_back(std::make_pair(theme->name, theme->name));
    }
    return themePairs;
}
