#include "ScreenshotService.h"
#include "../helpers/PathHelper.h"
#include "matoya.h"
#include <vector>
#include <fstream>
#include <Windows.h>
#include <wincodec.h>
#include <objidl.h>
#include <propidl.h>
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

    bool captureResizedBgraFromConfiguredMonitor(int targetWidth, int targetHeight, std::vector<uint8_t>& pixelData) {
        if (targetWidth <= 0 || targetHeight <= 0) {
            return false;
        }

        MonitorInfo info = { Config::cfg.video.monitor, 0, NULL, {0,0,0,0} };
        EnumDisplayMonitors(NULL, NULL, MonitorEnumProc, reinterpret_cast<LPARAM>(&info));

        if (!info.hMonitor) {
            // Fallback to primary monitor if the configured monitor index is out of bounds.
            info.hMonitor = MonitorFromWindow(NULL, MONITOR_DEFAULTTOPRIMARY);
            MONITORINFO mi = { sizeof(mi) };
            if (GetMonitorInfo(info.hMonitor, &mi)) {
                info.rect = mi.rcMonitor;
            }
            else {
                return false;
            }
        }

        const int sourceWidth = info.rect.right - info.rect.left;
        const int sourceHeight = info.rect.bottom - info.rect.top;
        if (sourceWidth <= 0 || sourceHeight <= 0) {
            return false;
        }

        HDC hdcScreen = GetDC(NULL);
        if (!hdcScreen) {
            return false;
        }

        HDC hdcMem = CreateCompatibleDC(hdcScreen);
        if (!hdcMem) {
            ReleaseDC(NULL, hdcScreen);
            return false;
        }

        HBITMAP hBitmap = CreateCompatibleBitmap(hdcScreen, targetWidth, targetHeight);
        if (!hBitmap) {
            DeleteDC(hdcMem);
            ReleaseDC(NULL, hdcScreen);
            return false;
        }

        SelectObject(hdcMem, hBitmap);

        // Resize source monitor content to the requested output dimensions.
        SetStretchBltMode(hdcMem, HALFTONE);
        StretchBlt(
            hdcMem,
            0,
            0,
            targetWidth,
            targetHeight,
            hdcScreen,
            info.rect.left,
            info.rect.top,
            sourceWidth,
            sourceHeight,
            SRCCOPY
        );

        BITMAPINFOHEADER bmih = { 0 };
        bmih.biSize = sizeof(BITMAPINFOHEADER);
        bmih.biWidth = targetWidth;
        bmih.biHeight = -targetHeight; // top-down
        bmih.biPlanes = 1;
        bmih.biBitCount = 32;
        bmih.biCompression = BI_RGB;

        const int pixelDataSize = targetWidth * targetHeight * 4;
        pixelData.resize(pixelDataSize);

        const int copiedLines = GetDIBits(
            hdcMem,
            hBitmap,
            0,
            targetHeight,
            pixelData.data(),
            reinterpret_cast<BITMAPINFO*>(&bmih),
            DIB_RGB_COLORS
        );

        DeleteObject(hBitmap);
        DeleteDC(hdcMem);
        ReleaseDC(NULL, hdcScreen);

        return copiedLines == targetHeight;
    }

    bool encodeBgraToJpegWic(const std::vector<uint8_t>& pixelData, int width, int height, int quality, std::vector<uint8_t>& jpegData) {
        if (pixelData.empty() || width <= 0 || height <= 0) {
            return false;
        }

        jpegData.clear();

        const int clampedQuality = max(1, min(quality, 100));

        const HRESULT initHr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
        const bool comReady = SUCCEEDED(initHr) || initHr == RPC_E_CHANGED_MODE;
        const bool shouldUninitialize = SUCCEEDED(initHr);
        if (!comReady) {
            return false;
        }

        IWICImagingFactory* factory = nullptr;
        IWICBitmap* sourceBitmap = nullptr;
        IWICFormatConverter* converter = nullptr;
        IStream* stream = nullptr;
        IWICBitmapEncoder* encoder = nullptr;
        IWICBitmapFrameEncode* frame = nullptr;
        IPropertyBag2* propertyBag = nullptr;

        bool ok = false;
        const UINT stride = static_cast<UINT>(width * 4);

        do {
            HRESULT hr = CoCreateInstance(
                CLSID_WICImagingFactory,
                nullptr,
                CLSCTX_INPROC_SERVER,
                IID_PPV_ARGS(&factory)
            );
            if (FAILED(hr)) break;

            hr = factory->CreateBitmapFromMemory(
                static_cast<UINT>(width),
                static_cast<UINT>(height),
                GUID_WICPixelFormat32bppBGRA,
                stride,
                static_cast<UINT>(pixelData.size()),
                const_cast<BYTE*>(pixelData.data()),
                &sourceBitmap
            );
            if (FAILED(hr)) break;

            hr = factory->CreateFormatConverter(&converter);
            if (FAILED(hr)) break;

            hr = converter->Initialize(
                sourceBitmap,
                GUID_WICPixelFormat24bppBGR,
                WICBitmapDitherTypeNone,
                nullptr,
                0.0,
                WICBitmapPaletteTypeCustom
            );
            if (FAILED(hr)) break;

            hr = CreateStreamOnHGlobal(nullptr, TRUE, &stream);
            if (FAILED(hr)) break;

            hr = factory->CreateEncoder(GUID_ContainerFormatJpeg, nullptr, &encoder);
            if (FAILED(hr)) break;

            hr = encoder->Initialize(stream, WICBitmapEncoderNoCache);
            if (FAILED(hr)) break;

            hr = encoder->CreateNewFrame(&frame, &propertyBag);
            if (FAILED(hr)) break;

            if (propertyBag != nullptr) {
                PROPBAG2 option = {};
                option.pstrName = const_cast<LPOLESTR>(L"ImageQuality");

                VARIANT value;
                VariantInit(&value);
                value.vt = VT_R4;
                value.fltVal = static_cast<float>(clampedQuality) / 100.0f;
                propertyBag->Write(1, &option, &value);
                VariantClear(&value);
            }

            hr = frame->Initialize(propertyBag);
            if (FAILED(hr)) break;

            hr = frame->SetSize(static_cast<UINT>(width), static_cast<UINT>(height));
            if (FAILED(hr)) break;

            WICPixelFormatGUID targetFormat = GUID_WICPixelFormat24bppBGR;
            hr = frame->SetPixelFormat(&targetFormat);
            if (FAILED(hr)) break;

            hr = frame->WriteSource(converter, nullptr);
            if (FAILED(hr)) break;

            hr = frame->Commit();
            if (FAILED(hr)) break;

            hr = encoder->Commit();
            if (FAILED(hr)) break;

            STATSTG stat = {};
            hr = stream->Stat(&stat, STATFLAG_NONAME);
            if (FAILED(hr) || stat.cbSize.HighPart != 0) break;

            const ULONG imageSize = stat.cbSize.LowPart;
            if (imageSize == 0) break;

            LARGE_INTEGER zero = {};
            hr = stream->Seek(zero, STREAM_SEEK_SET, nullptr);
            if (FAILED(hr)) break;

            jpegData.resize(imageSize);
            ULONG bytesRead = 0;
            hr = stream->Read(jpegData.data(), imageSize, &bytesRead);
            if (FAILED(hr) || bytesRead != imageSize) {
                jpegData.clear();
                break;
            }

            ok = true;
        } while (false);

        if (propertyBag) propertyBag->Release();
        if (frame) frame->Release();
        if (encoder) encoder->Release();
        if (stream) stream->Release();
        if (converter) converter->Release();
        if (sourceBitmap) sourceBitmap->Release();
        if (factory) factory->Release();

        if (shouldUninitialize) {
            CoUninitialize();
        }

        return ok;
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

bool ScreenshotService::captureJpeg(std::vector<uint8_t>& jpegData, int targetWidth, int targetHeight, int quality)
{
    std::vector<uint8_t> pixelData;
    if (!captureResizedBgraFromConfiguredMonitor(targetWidth, targetHeight, pixelData)) {
        return false;
    }

    return encodeBgraToJpegWic(pixelData, targetWidth, targetHeight, quality, jpegData);
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
        }
        else {
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

    std::vector<uint8_t> pixelData;
    if (!captureResizedBgraFromConfiguredMonitor(width, height, pixelData)) {
        return {};
    }

    BITMAPINFOHEADER bmih = {0};
    bmih.biSize = sizeof(BITMAPINFOHEADER);
    bmih.biWidth = width;
    bmih.biHeight = -height; // top-down
    bmih.biPlanes = 1;
    bmih.biBitCount = 32;
    bmih.biCompression = BI_RGB;

    int bmpDataSize = width * height * 4;

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