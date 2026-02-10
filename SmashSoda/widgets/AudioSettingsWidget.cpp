#include "AudioSettingsWidget.h"
#include "../services/ThemeController.h"

AudioSettingsWidget::AudioSettingsWidget(Hosting& hosting)
    : _hosting(hosting), _audioIn(_hosting.audioIn), _audioOut(_hosting.audioOut),
    _inputs(_hosting.audioIn.getDevices()), _outputs(_hosting.audioOut.getDevices())
{
}

bool AudioSettingsWidget::render(bool& showWindow)
{
    static float indentSize = 0;
    static ImVec2 cursor;
    static bool showPlot = false;

    // Implot
    static const int COUNT = AUDIOSRC_BUFFER_SIZE >> 2;
    static const ImPlotRange RANGE(0, COUNT);
    static const double SCALE = 32768.0;

    startWidget("Audio", showWindow, 450, 450, 450, 450);
    startBody(false);

    if (!_hosting.isRunning() && _hosting.isReady()) {
        _audioIn.captureAudio();
        _audioOut.captureAudio();
    }


    // =============================================================
    //  Input devices
    // =============================================================
    static size_t& currentInputDevice = _audioIn.currentDevice.index;
    ImGui::SetNextItemWidth(size.x);

    AppFonts::pushTitle();
    AppColors::pushInput();
    ImGui::Text("Microphone");
    AppColors::pop();
    AppFonts::pop();

    ImGui::Dummy(ImVec2(0.0f, 5.0f));

    std::vector<std::pair<std::string, std::string>> inputOptions;
    for (size_t i = 0; i < _inputs.size(); ++i) {
        inputOptions.push_back({ to_string(i), _inputs[i].name });
    }

    string inputCallback = to_string(currentInputDevice);
    if (elSelect("Input Source", inputOptions, inputCallback, "Set your microphone device. Only devices that support at least frequency 48000hz can be used.")) {
        currentInputDevice = stoul(inputCallback);
        _audioIn.setDevice(currentInputDevice);
        Config::cfg.audio.inputDevice = currentInputDevice;
        Config::cfg.Save();
    }

    elLabel("Volume");

    static int micVolume;
    static float micPreview, targetPreview;
    micVolume = (int)(100.0f * _audioIn.volume);
    targetPreview = AudioTools::decibelToFloat(_audioIn.popPreviewDecibel());
    micPreview = lerp(micPreview, targetPreview, easing(targetPreview - micPreview));
    if (AudioControlWidget::render("Microphone##Audio In", &micVolume, _audioIn.isEnabled, micPreview, AppIcons::micOn, AppIcons::micOff))
    {
        _audioIn.isEnabled = !_audioIn.isEnabled;
        Config::cfg.audio.micEnabled = _audioIn.isEnabled;
    }
    _audioIn.volume = (float)micVolume / 100.0f;
    Config::cfg.audio.micVolume = (float)micVolume;

    ImGui::Dummy(ImVec2(0, 20));

    if (showPlot)
    {
        if (ImPlot::BeginPlot("Microphone"))
        {
            ImPlot::SetupAxes(NULL, NULL, ImPlotAxisFlags_Lock, ImPlotAxisFlags_Lock);
            ImPlot::SetupAxesLimits(0, COUNT, -SCALE, SCALE);
            ImPlot::PlotShaded("Input levels", _audioIn.getPlot(), COUNT);
            ImPlot::EndPlot();
        }
    }

    // =============================================================
    //  Output devices
    // =============================================================
    AppFonts::pushTitle();
    AppColors::pushInput();
    ImGui::Text("Speakers");
    AppColors::pop();
    AppFonts::pop();

    ImGui::Dummy(ImVec2(0.0f, 5.0f));

    static size_t& currentOutputDevice = _audioOut.currentDevice.index;

    std::vector<std::pair<std::string, std::string>> outputOptions;
    for (size_t i = 0; i < _outputs.size(); ++i) {
		outputOptions.push_back({ to_string(i), _outputs[i].name });
	}

    string outputCallback = to_string(currentOutputDevice);
    if (elSelect("Output Source", outputOptions, outputCallback, "Set your output device. We recommend 48000hz for best compatibility.")) {
		currentOutputDevice = stoul(outputCallback);
		_audioOut.setDevice(currentOutputDevice);
		Config::cfg.audio.outputDevice = currentOutputDevice;
		Config::cfg.Save();
	}

    elLabel("Volume");

    ImGui::Dummy(ImVec2(0.0f, 1.0f));

    static int speakersVolume;
    static float speakersPreview;
    speakersVolume = (int)(100.0f *_audioOut.volume);
    targetPreview = AudioTools::decibelToFloat(_audioOut.popPreviewDecibel());
    speakersPreview = lerp(speakersPreview, targetPreview, easing(targetPreview - speakersPreview));
    if (AudioControlWidget::render("Speakers##Audio Out", &speakersVolume, _audioOut.isEnabled, speakersPreview, AppIcons::speakersOn, AppIcons::speakersOff))
    {
        _audioOut.isEnabled = !_audioOut.isEnabled;
        Config::cfg.audio.speakersEnabled = _audioOut.isEnabled;
    }
    _audioOut.volume = (float)speakersVolume / 100.0f;
    Config::cfg.audio.speakersVolume = (float)speakersVolume;

    ImGui::Dummy(ImVec2(0, 20));

    if (showPlot) {

        if (ImPlot::BeginPlot("Speakers")) {
            ImPlot::SetupAxes(NULL, NULL, ImPlotAxisFlags_Lock, ImPlotAxisFlags_Lock);
            ImPlot::SetupAxesLimits(0, COUNT, -SCALE, SCALE);
            ImPlot::PlotShaded("Output levels", _audioOut.getPlot(), COUNT);
            ImPlot::EndPlot();
        }

    }

    //ImGui::Checkbox("Plot Audio (debug for devs)", &showPlot);
    //_audioIn.togglePlot(showPlot);
    //_audioOut.togglePlot(showPlot);


    endBody();
    endWidget();

    return true;
}

float AudioSettingsWidget::lerp(float val1, float val2, float t)
{
    return (1-t) * val1 + t * val2;
}

float AudioSettingsWidget::easing(float t)
{
    return 0.3f * max(abs(t), 0.4f);
}


