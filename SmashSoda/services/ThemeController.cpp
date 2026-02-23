#include "ThemeController.h"
#include <imgui/imgui.h>
#include <algorithm>
#include <cctype>
#include <cmath>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace {
    std::string toLower(std::string value) {
        std::transform(value.begin(), value.end(), value.begin(), [](unsigned char c) {
            return static_cast<char>(std::tolower(c));
        });
        return value;
    }

    bool endsWithJson(const std::string& value) {
        const std::string lower = toLower(value);
        return lower.size() >= 5 && lower.substr(lower.size() - 5) == ".json";
    }

    std::string basenameWithoutExtension(const std::string& value) {
        const size_t dot = value.find_last_of('.');
        if (dot == std::string::npos) {
            return value;
        }
        return value.substr(0, dot);
    }
}

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
    addTheme(std::make_unique<WildfireTheme>());
}

void ThemeController::loadCustomThemes() {
    const std::string themesPath = PathHelper::GetCurrentPath() + "\\themes\\";

    MTY_FileList* files = MTY_GetFileList(themesPath.c_str(), NULL);
    if (files == nullptr) {
        return;
    }

    for (uint32_t i = 0; i < files->len; i++) {
        const MTY_FileDesc& file = files->files[i];

        if (file.dir || file.name == nullptr || !endsWithJson(file.name)) {
            continue;
        }

        std::string filepath = file.path == nullptr ? "" : file.path;
        if (!filepath.empty()) {
            const char sep = filepath.back();
            if (sep != '\\' && sep != '/') {
                filepath += "\\";
            }
        }
        filepath += file.name;

        size_t size = 0;
        void* data = MTY_ReadFile(filepath.c_str(), &size);
        if (data == nullptr || size == 0) {
            if (data != nullptr) {
                MTY_Free(data);
            }
            continue;
        }

        try {
            const std::string content(static_cast<char*>(data), size);
            const json parsed = json::parse(content);
            const std::string fallbackName = basenameWithoutExtension(file.name);
            const std::string themeName = parsed.contains("name") && parsed["name"].is_string()
                ? parsed["name"].get<std::string>()
                : fallbackName;

            addTheme(std::make_unique<Theme>(themeName, parsed));
        }
        catch (const std::exception&) {
        }

        MTY_Free(data);
    }

    MTY_FreeFileList(&files);
}

void ThemeController::addTheme(std::unique_ptr<Theme> theme) {
    if (theme == nullptr) {
        return;
    }

    for (auto& existingTheme : _themes) {
        if (existingTheme->name == theme->name) {
            existingTheme = std::move(theme);
            return;
        }
    }

    _themes.push_back(std::move(theme));
}

void ThemeController::applyTheme(const std::string& name) {
    for (auto& theme : _themes) {
        if (theme->name == name) {
            _activeTheme = theme.get();
            // Only apply if ImGui context is ready
            if (ImGui::GetCurrentContext() != nullptr) {
                _activeTheme->apply();
                ImGui::GetStyle().ScaleAllSizes(_uiScale);
            }
            break;
        }
    }
}

void ThemeController::setUiScale(float scale) {
    float clampedScale = scale;
    if (clampedScale < 0.5f) {
        clampedScale = 0.5f;
    }
    else if (clampedScale > 3.0f) {
        clampedScale = 3.0f;
    }
    if (std::fabs(clampedScale - _uiScale) < 0.001f) {
        return;
    }

    _uiScale = clampedScale;

    if (_activeTheme != nullptr && ImGui::GetCurrentContext() != nullptr) {
        _activeTheme->apply();
        ImGui::GetStyle().ScaleAllSizes(_uiScale);
    }
}

float ThemeController::getUiScale() const {
    return _uiScale;
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
