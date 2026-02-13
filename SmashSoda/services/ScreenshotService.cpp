#include "ScreenshotService.h"
#include "../helpers/PathHelper.h"
#include "matoya.h"
#include <vector>
#include <fstream>
#include <Windows.h>
#include "../core/Config.h"
#include "../Hosting.h"

extern Hosting g_hosting;

ScreenshotService ScreenshotService::_instance;

namespace {
    struct MonitorInfo {
        UINT targetIndex;
        UINT currentIndex;
        HMONITOR hMonitor;
        RECT rect;
    };

    BOOL CALLBACK MonitorEnumProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData) {
        MonitorInfo* pInfo = reinterpret_cast<MonitorInfo*>(dwData);
        if (pInfo->currentIndex == pInfo->targetIndex) {
            pInfo->hMonitor = hMonitor;
            pInfo->rect = *lprcMonitor;
            return FALSE; // Stop enumerating
        }
        pInfo->currentIndex++;
        return TRUE; // Continue enumerating
    }
}

ScreenshotService& ScreenshotService::getInstance()
{
    return _instance;
}

ScreenshotService::ScreenshotService()
{
}

ScreenshotService::~ScreenshotService()
{
}

bool ScreenshotService::takeScreenshotToFile(const std::string& filename)
{
    int width, height;
    std::vector<uint8_t> bmpData = getScreenshotData(width, height);

    if (bmpData.empty()) {
        g_hosting.logMessage("Failed to take screenshot: no data");
        return false;
    }

    std::string path = PathHelper::GetConfigPath() + "screenshots\\";
    if (!MTY_FileExists(path.c_str())) {
		MTY_Mkdir(path.c_str());
	}
    path += filename;

    std::ofstream file(path, std::ios::out | std::ios::binary);
    if (!file) {
        g_hosting.logMessage("Failed to create screenshot file: " + path);
        return false;
    }

    file.write(reinterpret_cast<const char*>(bmpData.data()), bmpData.size());
    file.close();
    g_hosting.logMessage("Screenshot saved to " + path);

    return true;
}

bool ScreenshotService::takeScreenshotToApi(const std::string& apiEndpoint)
{
    int width, height;
    std::vector<uint8_t> bmpData = getScreenshotData(width, height);

    if (bmpData.empty()) {
        return false;
    }

    void* response = nullptr;
    size_t responseSize = 0;
    uint16_t status = 0;

    std::string domain = apiEndpoint;
    std::string path = "/";
    size_t pos = apiEndpoint.find("/");
    if (pos != std::string::npos) {
        domain = apiEndpoint.substr(0, pos);
        path = apiEndpoint.substr(pos);
    }

    std::string headers = "Content-Type: image/bmp\r\n";
    headers += "Content-Length: " + std::to_string(bmpData.size()) + "\r\n";

    const bool success = MTY_HttpRequest(
        domain.c_str(), 0, true, "POST", path.c_str(),
        headers.c_str(),
        bmpData.data(), bmpData.size(), 20000,
        &response, &responseSize, &status
    );

    if (response) {
        MTY_Free(response);
    }

    return success && status >= 200 && status < 300;
}

std::vector<uint8_t> ScreenshotService::getScreenshotData(int& width, int& height)
{
    const int THUMBNAIL_WIDTH = 480;

    MonitorInfo info = { Config::cfg.video.monitor, 0, NULL, {0,0,0,0} };
    EnumDisplayMonitors(NULL, NULL, MonitorEnumProc, reinterpret_cast<LPARAM>(&info));

    if (!info.hMonitor) {
        // Fallback to primary monitor if index is out of bounds
        info.hMonitor = MonitorFromWindow(NULL, MONITOR_DEFAULTTOPRIMARY);
        MONITORINFO mi = { sizeof(mi) };
        if (GetMonitorInfo(info.hMonitor, &mi)) {
            info.rect = mi.rcMonitor;
        } else {
            return {}; // Could not get any monitor info
        }
    }

    int screenWidth = info.rect.right - info.rect.left;
    int screenHeight = info.rect.bottom - info.rect.top;

    // Calculate thumbnail height to maintain aspect ratio
    int thumbnailHeight = (int)(screenHeight * (THUMBNAIL_WIDTH / (float)screenWidth));

    // Update output width and height
    width = THUMBNAIL_WIDTH;
    height = thumbnailHeight;

    HDC hdcScreen = GetDC(NULL);
    if (!hdcScreen) return {};

    HDC hdcMem = CreateCompatibleDC(hdcScreen);
    if (!hdcMem) {
        ReleaseDC(NULL, hdcScreen);
        return {};
    }

    HBITMAP hBitmap = CreateCompatibleBitmap(hdcScreen, width, height);
    if (!hBitmap) {
        DeleteDC(hdcMem);
        ReleaseDC(NULL, hdcScreen);
        return {};
    }

    SelectObject(hdcMem, hBitmap);

    // Use StretchBlt to resize the image
    SetStretchBltMode(hdcMem, HALFTONE);
    StretchBlt(hdcMem, 0, 0, width, height, hdcScreen, info.rect.left, info.rect.top, screenWidth, screenHeight, SRCCOPY);

    BITMAPINFOHEADER bmih = {0};
    bmih.biSize = sizeof(BITMAPINFOHEADER);
    bmih.biWidth = width;
    bmih.biHeight = -height; // top-down
    bmih.biPlanes = 1;
    bmih.biBitCount = 32;
    bmih.biCompression = BI_RGB;

    int bmpDataSize = width * height * 4;
    std::vector<uint8_t> pixelData(bmpDataSize);

    GetDIBits(hdcMem, hBitmap, 0, height, pixelData.data(), (BITMAPINFO*)&bmih, DIB_RGB_COLORS);

    DeleteObject(hBitmap);
    DeleteDC(hdcMem);
    ReleaseDC(NULL, hdcScreen);

    BITMAPFILEHEADER bmfh = {0};
    bmfh.bfType = 0x4D42; // 'BM'
    bmfh.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + bmpDataSize;
    bmfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

    std::vector<uint8_t> bmpFileData(bmfh.bfSize);
    memcpy(bmpFileData.data(), &bmfh, sizeof(BITMAPFILEHEADER));
    memcpy(bmpFileData.data() + sizeof(BITMAPFILEHEADER), &bmih, sizeof(BITMAPINFOHEADER));
    memcpy(bmpFileData.data() + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER), pixelData.data(), bmpDataSize);

    return bmpFileData;
}