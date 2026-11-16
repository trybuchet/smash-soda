#pragma once

#include <string>
#include <vector>

class ScreenshotService
{
public:
    static ScreenshotService& getInstance();
    bool takeScreenshotToFile(const std::string& filename);
    bool takeScreenshotToApi(const std::string& apiEndpoint);
    // Captures the selected monitor and encodes it to JPEG in memory.
    bool captureJpeg(std::vector<uint8_t>& jpegData, int targetWidth = 300, int targetHeight = 150, int quality = 85);

private:
    ScreenshotService();
    ~ScreenshotService();
    ScreenshotService(const ScreenshotService&) = delete;
    ScreenshotService& operator=(const ScreenshotService&) = delete;

    std::vector<uint8_t> getScreenshotData(int& width, int& height);

    static ScreenshotService _instance;
};
