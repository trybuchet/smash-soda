#pragma once

#include <functional>

struct Tab {
    const char* name;
    std::function<void()> render;
};