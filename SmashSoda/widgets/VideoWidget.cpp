#include "VideoWidget.h"

VideoWidget::VideoWidget(Hosting& hosting)
    : _hosting(hosting), _dx11(_hosting.getDX11())
{
    _fps = (int)Config::cfg.video.fps;
    _bandwidth = (int)Config::cfg.video.bandwidth;
}

bool VideoWidget::render(bool& showWindow)
{
    static float indentSize = 0;
    static ImVec2 dummySize = ImVec2(0.0f, 5.0f);
    static ImVec2 cursor;

    //ImGui::SetNextWindowSizeConstraints(ImVec2(400, 400), ImVec2(800, 900));

    startWidget("Video", showWindow, 450, 450, 450, 450);
    startBody(false);

    // =========================================================
    // Monitors
    // =========================================================
    static UINT currentScreen = 0;
    currentScreen = _dx11.getScreen();

    static vector<string> screens = _dx11.listScreens();
    static string screenName = (currentScreen < screens.size()) ? screens[currentScreen] : "";

    std::vector<std::pair<std::string, std::string>> screenOptions;
    for (size_t i = 0; i < screens.size(); ++i) {
		screenOptions.push_back({ to_string(i), screens[i] });
	}

    string screenCallback = to_string(currentScreen);
    if (elSelect("Monitor", screenOptions, screenCallback, "Choose the screen you want to display.")) {
		currentScreen = stoul(screenCallback);
		screenName = screens[currentScreen];
		_dx11.setScreen(currentScreen);
	}

    // =========================================================
    // Adapters
    // =========================================================
    static size_t currentGPU = 0;
    currentGPU = _dx11.getGPU();

    static vector<string> gpus = _dx11.listGPUs();
    static string gpuName = (currentGPU < gpus.size()) ? gpus[currentGPU] : "";

    std::vector<std::pair<std::string, std::string>> gpuOptions;
    for (size_t i = 0; i < gpus.size(); ++i) {
		gpuOptions.push_back({ to_string(i), gpus[i] });
	}

    string gpuCallback = to_string(currentGPU);
    if (elSelect("Adapter", gpuOptions, gpuCallback, "Choose the GPU that renders your desktop.")) {
        currentGPU = stoul(gpuCallback);
		gpuName = gpus[currentGPU];
		_dx11.setGPU(currentGPU);
		screens = _dx11.listScreens();
		_dx11.setScreen(currentScreen);
		currentScreen = _dx11.getScreen();
		screenName = currentScreen >= screens.size() ? "" : screens[currentScreen];
	}

    // =========================================================
    // Resolution
    // =========================================================
    static unsigned int currentResolutionIndex = Config::cfg.video.resolutionIndex;
    currentResolutionIndex = Config::cfg.video.resolutionIndex;

    std::vector<std::pair<std::string, std::string>> resolutionOptions;
    for (size_t i = 0; i < Config::cfg.resolutions.size(); ++i) {
        resolutionOptions.push_back({ to_string(i), Config::cfg.resolutions[i].label });
    }

    string resolutionCallback = to_string(currentResolutionIndex);
    if (elSelect("Resolution", resolutionOptions, resolutionCallback, "Choose the output resolution for the stream. 'Keep Host Resolution' uses the native monitor resolution.")) {
        currentResolutionIndex = stoul(resolutionCallback);
        Config::cfg.video.resolutionIndex = currentResolutionIndex;
        
        // Apply the resolution to DX11
        int width = Config::cfg.resolutions[currentResolutionIndex].width;
        int height = Config::cfg.resolutions[currentResolutionIndex].height;
        _dx11.setTargetResolution(width, height);
        
        Config::cfg.Save();
    }

    // =========================================================
    // Bandwidth
    // =========================================================
    static int previousBandwidth;
    static int previousFps;
    previousBandwidth = _bandwidth;
    previousFps = _fps;

    cursor = ImGui::GetCursorPos();
    ImGui::SetNextItemWidth(size.x);

    /*ImGui::BeginGroup();
    AppStyle::pushLabel();
    ImGui::Text("Bitrate");
    AppStyle::pop();
    if (IntRangeWidget::render("Bandwidth (Mbps)", _bandwidth, 1, 1000, 1))
    {
        TitleTooltipWidget::render("Encoder Bandwidth", "Limits the encoder bandwidth usage.\n(This value is split between guests.)");
    }
    ImGui::EndGroup();*/
	if (elNumber("Bandwidth (Mbps)", _bandwidth, 1, 1000, "Limits the encoder bandwidth usage.\n(This value is split between guests.")) {
		TitleTooltipWidget::render("Encoder Bandwidth", "Limits the encoder bandwidth usage.\n(This value is split between guests.)");
	}

    // =========================================================
    // FPS
    // =========================================================
    /*ImGui::BeginGroup();
    AppStyle::pushLabel();
    ImGui::Text("FPS");
    AppStyle::pop();
    if (IntRangeWidget::render("Encoder FPS", _fps, 10, 250, 0.5f)) {
        TitleTooltipWidget::render("Encoder FPS", "Limits the amount of frames per second.");
    }
    ImGui::EndGroup();

    AppStyle::pop();
    ImGui::End();
    AppStyle::pop();*/
	if (elNumber("FPS", _fps, 10, 250, "Limits the amount of frames per second.")) {
		TitleTooltipWidget::render("Encoder FPS", "Limits the amount of frames per second.");
    }

    static Debouncer debouncer(DEBOUNCER_TIME_MS, [&]() {
        _hosting.setHostVideoConfig(_fps, _bandwidth);
        _hosting.applyHostConfig();
        Config::cfg.Save();
    });
    if (previousFps != _fps || previousBandwidth != _bandwidth)
    {
        debouncer.start();
    }

    endBody();
    endWidget();

    return true;
}


