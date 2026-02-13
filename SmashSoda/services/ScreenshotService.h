#pragma once

#include <string>
#include <vector>

class ScreenshotService
{
public:
    static ScreenshotService& getInstance();
    bool takeScreenshotToFile(const std::string& filename);
    bool takeScreenshotToApi(const std::string& apiEndpoint);

private:
    ScreenshotService();
    ~ScreenshotService();
    ScreenshotService(const ScreenshotService&) = delete;
    ScreenshotService& operator=(const ScreenshotService&) = delete;

    std::vector<uint8_t> getScreenshotData(int& width, int& height);

    static ScreenshotService _instance;
};
