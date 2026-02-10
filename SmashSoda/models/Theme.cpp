#include "Theme.h"
#include "imgui/imgui.h"

Theme::Theme(const string& name, const json& theme)
    : name(name) {

    // Load theme from JSON if provided
    if (!theme.empty()) {
        loadFromJSON(theme);
    }

}

bool Theme::loadFromJSON(const json& theme) {
    return true;
}

void Theme::apply() {
    
}

// Function to convert a color hex string to ImVec4
ImVec4 Theme::hexToImVec4(const std::string& hex) {
    ImVec4 color;
    color.x = (float)std::stoul(hex.substr(1, 2), nullptr, 16) / 255.0f;
    color.y = (float)std::stoul(hex.substr(3, 2), nullptr, 16) / 255.0f;
    color.z = (float)std::stoul(hex.substr(5, 2), nullptr, 16) / 255.0f;
    color.w = (float)std::stoul(hex.substr(7, 2), nullptr, 16) / 255.0f;
    return color;
}

// Function to convert a rgba() value to ImVec4
ImVec4 Theme::rgbaToImVec4(const std::string& rgba) {
    std::istringstream iss(rgba);
    int r, g, b, a;
    iss >> std::hex >> r >> g >> b >> a;
    ImVec4 color;
    color.x = (float)r / 255.0f;
    color.y = (float)g / 255.0f;
    color.z = (float)b / 255.0f;
    color.w = (float)a / 255.0f;
    return color;
}

// Function to parse a string to see if it's a hex or rgba() value
ImVec4 Theme::parseColorString(const std::string& colorString, ImVec4 defaultColor) {
    ImVec4 color;
    if (colorString.size() == 7 && isxdigit(colorString[0]) && isxdigit(colorString[1]) && isxdigit(colorString[2]) && isxdigit(colorString[3]) && isxdigit(colorString[4]) && isxdigit(colorString[5]) && colorString[0] == '#' && colorString[6] == '\0') {
        color = hexToImVec4(colorString);
    } else if (colorString.find("rgba(") == 0 && colorString.find(")") == colorString.size() - 1) {
        std::string rgba = colorString.substr(5, colorString.size() - 6);
        color = rgbaToImVec4(rgba);
    } else {
        color = defaultColor;
    }
    return color;
}