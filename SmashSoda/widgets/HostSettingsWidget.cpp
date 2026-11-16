#include "HostSettingsWidget.h"
#include "../ImGui/imform.h"
#include "../services/OverlayService.h"

HostSettingsWidget::HostSettingsWidget(Hosting& hosting, function<void(bool)> onHostRunningStatusCallback)
    : _hosting(hosting), _audioIn(_hosting.audioIn), _audioOut(_hosting.audioOut), _onHostRunningStatusCallback(onHostRunningStatusCallback)
{
    ParsecHostConfig cfg = hosting.getHostConfig();
    try
    {
        strcpy_s(_gameID, cfg.gameID);
        strcpy_s(_roomName, (Config::cfg.room.name.size() < 255 ? Config::cfg.room.name.c_str() : ""));
        strcpy_s(_gameName, (Config::cfg.room.game.size() < 255 ? Config::cfg.room.game.c_str() : ""));
        strcpy_s(_description, (Config::cfg.room.details.size() <= 500 ? Config::cfg.room.details.c_str() : ""));
        strcpy_s(_secret, cfg.secret);
        strcpy_s(_kioskApplication, "");
        strcpy_s(_kioskParam, "");
        strcpy_s(_streamUrl, (Config::cfg.room.streamUrl.size() < 255 ? Config::cfg.room.streamUrl.c_str() : ""));
    }
    catch (const std::exception&)
    {
        try
        {
            strcpy_s(_gameID, "");
            strcpy_s(_roomName, "");
			strcpy_s(_gameName, "");
			strcpy_s(_description, "");
            strcpy_s(_secret, "");
            strcpy_s(_kioskApplication, "");
            strcpy_s(_kioskParam, "");
            strcpy_s(_streamUrl, "");
        } catch (const std::exception&) {}
    }
    _maxGuests = Config::cfg.room.guestLimit;
    _repThreshold = Config::cfg.room.repThreshold;
    
    _micVolume = Config::cfg.audio.micVolume;
    _audioIn.volume = (float)_micVolume / 100.0f;
    
    _speakersVolume = Config::cfg.audio.speakersVolume;
    _audioOut.volume = (float)_speakersVolume / 100.0f;

    _audioIn.isEnabled = Config::cfg.audio.micEnabled;
    _audioOut.isEnabled = Config::cfg.audio.speakersEnabled;

    _latencyLimiter = Config::cfg.room.latencyLimit;
    _latencyLimit = Config::cfg.room.latencyLimitThreshold;

    _hotseat = Config::cfg.hotseat.enabled;

    _kioskMode = Config::cfg.kioskMode.enabled;

    _libraryGame = "Default";
    Arcade::instance.artworkID = Config::cfg.room.artworkID;
    _selectedArtworkValue = std::to_string(Arcade::instance.artworkID);

    if (strlen(_secret) == 0) {
        try { strcpy_s(_secret, "play-now"); }
        catch (const std::exception&) {}
    }
    updateSecretLink();

}

/// <summary>
/// Validate settings before sending to Soda Arcade.
/// </summary>
/// <returns></returns>
bool HostSettingsWidget::validateSettings() {

	bool isValid = true;

	// Clear previous errors
	_roomNameError = "";

    if (strlen(_roomName) > 50) {
        _roomNameError = "Room name must be less than 50 characters.";
		isValid = false;
	}

    if (strlen(_gameName) > 50) {
        _validateError = "Game name must be less than 50 characters.";
        _showErrorPopup = true;
        ImGui::OpenPopup("ERROR");
		return false;
	}

	// Details can't be longer than 500 characters
	if (strlen(_description) > 500) {
        _validateError = "Description can't be longer than 500 characters.";
        _showErrorPopup = true;
        ImGui::OpenPopup("ERROR");
		return false;
	}

	return isValid;
    
}

bool HostSettingsWidget::render(bool& showWindow, HWND& hwnd) {

    startWidget("Hosting", showWindow, 422, 10, 800, 1000, 350, 400);

    startBody(true);

    // Prepare games list
    std::vector<std::pair<std::string, std::string>> games;
    std::pair<std::string, std::string> defaultGame;
    defaultGame.first = "Default";
    defaultGame.second = "Default";
    games.push_back(defaultGame);
    for (int i = 0; i < Cache::cache.gameList.getGames().size(); ++i) {
        std::pair<std::string, std::string> game;
        game.first = Cache::cache.gameList.getGames()[i].name;
        game.second = Cache::cache.gameList.getGames()[i].name;
        games.push_back(game);
    }

    try {
        if (elSelect("Library", games, _libraryGame,
            "Pre-configure your room for a specific game in your library.")) {

            // Find game in library
            for (int i = 0; i < Cache::cache.gameList.getGames().size(); ++i) {
                if (Cache::cache.gameList.getGames()[i].name == _libraryGame) {
                    strcpy_s(_gameName, Cache::cache.gameList.getGames()[i].name.c_str());
                    break;
                }
            }
        }
    }
    catch (const std::exception&) {

        _libraryGame = "Default";

        if (elSelect("Library", games, _libraryGame,
            "Pre-configure your room for a specific game in your library.")) {

            // Find game in library
            for (int i = 0; i < Cache::cache.gameList.getGames().size(); ++i) {
                if (Cache::cache.gameList.getGames()[i].name == _libraryGame) {
                    strcpy_s(_gameName, Cache::cache.gameList.getGames()[i].name.c_str());
                    break;
                }
            }
        }

    }

    if (elText("Room Name", _roomName, "The public name of your Soda Arcade room. Max 50 characters.")) {
        if (strlen(_roomName) > 50) {
            string name = _roomName;
            name = name.substr(0, 50);
            strcpy_s(_roomName, name.c_str());
        }
        if (_hosting.isRunning()) {
            _updated = true;
        }
    }

    // Keep share link in sync with runtime auth/session state.
    updateSecretLink();

    // Read only share link
    elReadOnly("Share Link", _shareLink, "The link to share with your guests.");

    // Guest limit
    if (elNumber("Guest Slots", _maxGuests, 0, 20, "The maximum number of guests that can join your room.")) {
        Config::cfg.room.guestLimit = _maxGuests;
        if (_hosting.isRunning()) {
            _updated = true;
        }
    }

    // Private room
    if (elCheckbox("Private Room", Config::cfg.room.privateRoom, "Whether to post your room on Soda Arcade.")) {
        // If public game is false, check we have authenticated with Soda Arcade
        if (!Config::cfg.room.privateRoom) {
            if (Arcade::instance.credentials.token.empty()) {
                Config::cfg.arcade.showLogin = true;
                Config::cfg.room.privateRoom = true;
            }
        }

        if (_hosting.isRunning()) {
            _updated = true;
        }

        updateSecretLink();
    }

    if (!Config::cfg.room.privateRoom) {
        if (elText("Game", _gameName, "The game name sent to Soda Arcade. Max 50 characters.")) {
            if (strlen(_gameName) > 50) {
                string name = _gameName;
                name = name.substr(0, 50);
                strcpy_s(_gameName, name.c_str());
            }
            if (_hosting.isRunning()) {
                _updated = true;
            }
        }

        // Mature (18-rated) rooms are only shown to, and joinable by, confirmed adults on Soda Arcade.
        const bool canHostMature = Arcade::instance.account.ageStatus == "adult";
        // Only clear a saved setting once we know the account isn't an adult (not while it's still loading).
        if (Arcade::instance.account.known && !canHostMature && Config::cfg.room.matureRoom) {
            Config::cfg.room.matureRoom = false;
        }
        ImGui::BeginDisabled(!canHostMature);
        if (elCheckbox(
            "Mature Room (18+)",
            Config::cfg.room.matureRoom,
            canHostMature
                ? "Tick this when you stream a game rated 18. Only users who have confirmed they're 18 or over can see or join your room on Soda Arcade."
                : "Only confirmed adults can host mature rooms."
        )) {
            Config::cfg.Save();
            if (_hosting.isRunning()) {
                _updated = true;
            }
        }
        ImGui::EndDisabled();

        // Room thumbnail: the same choices as the Soda Arcade app's "Room Thumbnail" setting.
        std::vector<std::pair<int, std::string>> previewTypeOptions = {
            { 0, "Room Snapshot" },
            { 1, "Game Cover Art" },
            { 2, "Custom Artwork" }
        };

        int previewTypeIndex = 1;
        if (Config::cfg.room.previewType == "snapshot") {
            previewTypeIndex = 0;
        }
        else if (Config::cfg.room.previewType == "custom") {
            previewTypeIndex = 2;
        }

        const char* previewHelp =
            previewTypeIndex == 0 ? "Shows a snapshot of your stream, updated every 10 minutes while your room is public."
            : previewTypeIndex == 1 ? "Shows the official cover art for the game above. If no cover is found, your room snapshot is shown instead."
            : "Shows one of the images you've uploaded to Soda Arcade. Recommended size: 300x150 pixels.";

        if (elRadio("Room Thumbnail", previewTypeOptions, previewTypeIndex, previewHelp)) {
            Config::cfg.room.previewType = previewTypeIndex == 0 ? "snapshot" : previewTypeIndex == 2 ? "custom" : "auto";
            // Fetch the artwork list again the next time Custom Artwork is shown.
            _requestedCustomArtworkFetch = false;
            Config::cfg.Save();
            if (_hosting.isRunning()) {
                _updated = true;
            }
        }

        if (Config::cfg.room.previewType == "custom") {
            Theme* theme = ThemeController::getInstance().getActiveTheme();

            if (!Arcade::instance.isReady()) {
                ImGui::PushStyleColor(ImGuiCol_Text, theme->warning);
                ImGui::TextWrapped("Log in to Soda Arcade and finish setting up your account to use your artwork.");
                ImGui::PopStyleColor();
                ImGui::Dummy(ImVec2(0.0f, 10.0f));
            }
            else {
                // Load the list once when this option is first shown (and on Refresh).
                // getArtwork() refills the list in place, so it runs on this thread rather than in the background.
                if (Arcade::instance.artwork.empty() && !_requestedCustomArtworkFetch) {
                    Arcade::instance.getArtwork();
                    _requestedCustomArtworkFetch = true;
                }

                if (Arcade::instance.artwork.empty()) {
                    ImGui::PushStyleColor(ImGuiCol_Text, theme->warning);
                    ImGui::TextWrapped("You haven't uploaded any artwork yet. Upload images in the Soda Arcade app (Create Room, Customize, Room Thumbnail, Custom Preview Image), then refresh. Until then your room uses Game Cover Art.");
                    ImGui::PopStyleColor();
                }
                else {
                    // Artwork options, with a placeholder until one is picked.
                    std::vector<std::pair<std::string, std::string>> artworkOptions;
                    artworkOptions.reserve(Arcade::instance.artwork.size() + 1);
                    artworkOptions.push_back({ "", "Select..." });

                    bool selectionFound = false;
                    for (const auto& art : Arcade::instance.artwork) {
                        artworkOptions.push_back({ std::to_string(art.id), art.title.empty() ? ("Artwork #" + std::to_string(art.id)) : art.title });
                        if (art.id == Arcade::instance.artworkID) {
                            selectionFound = true;
                        }
                    }

                    // A previously chosen image that's been deleted or isn't approved any more.
                    if (!selectionFound && Arcade::instance.artworkID != -1) {
                        Arcade::instance.artworkID = -1;
                        Config::cfg.room.artworkID = -1;
                    }
                    _selectedArtworkValue = selectionFound ? std::to_string(Arcade::instance.artworkID) : "";

                    if (elSelect("Artwork", artworkOptions, _selectedArtworkValue)) {
                        int selectedArtworkId = -1;
                        if (!_selectedArtworkValue.empty()) {
                            try {
                                selectedArtworkId = std::stoi(_selectedArtworkValue);
                            }
                            catch (const std::exception&) {
                                selectedArtworkId = -1;
                            }
                        }

                        Arcade::instance.artworkID = selectedArtworkId;
                        Config::cfg.room.artworkID = selectedArtworkId;
                        _selectedArtwork = selectedArtworkId;
                        Config::cfg.Save();

                        if (_hosting.isRunning()) {
                            _updated = true;
                        }
                    }

                    if (Arcade::instance.artworkID <= 0) {
                        ImGui::PushStyleColor(ImGuiCol_Text, theme->warning);
                        ImGui::TextWrapped("Pick an image, or your room will use Game Cover Art.");
                        ImGui::PopStyleColor();
                    }
                }

                if (elBtnSecondary("Refresh artwork")) {
                    Arcade::instance.getArtwork();
                    _requestedCustomArtworkFetch = true;
                }
                ImGui::Dummy(ImVec2(0.0f, 10.0f));
            }
        }

        // Tags multiselect
        {
            static const std::vector<std::pair<std::string, std::string>> tagOptions = {
                {"Action","Action"},{"Adventure","Adventure"},{"Arcade","Arcade"},
                {"Backseat Gamer","Backseat Gamer"},{"Battle Royale","Battle Royale"},
                {"Beginner Friendly","Beginner Friendly"},{"Best of 2","Best of 2"},
                {"Best of 3","Best of 3"},{"Best of 5","Best of 5"},
                {"Double Elimination","Double Elimination"},{"Board Game","Board Game"},
                {"Bullet Hell","Bullet Hell"},{"Card Game","Card Game"},{"Casual","Casual"},
                {"Chat","Chat"},{"Co-op","Co-op"},{"Competitive","Competitive"},
                {"Educational","Educational"},{"Fighting","Fighting"},
                {"First Person Shooter","First Person Shooter"},{"Hotseat","Hotseat"},
                {"Horror","Horror"},{"Idle","Idle"},{"Indie","Indie"},
                {"King of the Hill","King of the Hill"},{"Metroidvania","Metroidvania"},
                {"MMO","MMO"},{"MOBA","MOBA"},{"Multiplayer","Multiplayer"},
                {"Music","Music"},{"Open World","Open World"},{"Party Game","Party Game"},
                {"Platform Fighter","Platform Fighter"},{"Platformer","Platformer"},
                {"Puzzle","Puzzle"},{"Racing","Racing"},{"Retro","Retro"},
                {"Rhythm","Rhythm"},{"Role-Playing Game","Role-Playing Game"},
                {"Roguelike","Roguelike"},{"Sandbox","Sandbox"},{"Shooter","Shooter"},
                {"Side-scroller","Side-scroller"},{"Simulation","Simulation"},
                {"Single Elimination","Single Elimination"},{"Single Player","Single Player"},
                {"Sports","Sports"},{"Stealth","Stealth"},{"Strategy","Strategy"},
                {"Survival","Survival"},{"Swiss","Swiss"},
                {"Third Person Shooter","Third Person Shooter"},{"Tournament","Tournament"},
                {"Tower Defense","Tower Defense"},{"Trivia","Trivia"},
                {"Turn-based","Turn-based"},{"Twin Stick Shooter","Twin Stick Shooter"},
                {"Variety","Variety"},{"Visual Novel","Visual Novel"}
            };

            if (elMultiSelect("Tags", tagOptions, Config::cfg.room.tags, SIZE_MAX,
                "Add tags to help players find your room on Soda Arcade.")) {
                if (_hosting.isRunning()) {
                    _updated = true;
                }
            }
        }

        if (elText("Stream URL", _streamUrl, "If you're streaming on Twitch.tv, you can add your Twitch stream URL here to have it appear on Soda Arcade. Max 255 characters.")) {
            if (_hosting.isRunning()) {
                _updated = true;
            }
        }

        if (elTextArea("Room Details", _description, "Optional room details shown on Soda Arcade. Max 500 characters.")) {
            if (_hosting.isRunning()) {
                _updated = true;
            }
        }
    }

    // Hotseat
    elCheckbox("Hotseat", Config::cfg.hotseat.enabled, "Allow guests to take turns playing.");

    // Kiosk mode
    if (elCheckbox("Kiosk Mode", Config::cfg.kioskMode.enabled, "Automatically start the game when hosting.")) {
        // Can only start kiosk mode when a game is selected
        if (_libraryGame == "Default") {
            Config::cfg.kioskMode.enabled = false;
        } else {
            if (_hosting.isRunning()) {
                if (Config::cfg.kioskMode.enabled && _libraryGame != "Default") {
                    //ProcessMan::instance.start(Cache::cache.gameList.getGames()[_libraryID].path, Cache::cache.gameList.getGames()[_libraryID].parameters);
                }
            }
        }
    }

    if (elCheckbox("Overlay", Config::cfg.overlay.enabled, "Show the overlay when hosting to see chat and guest info (useful if you only have one monitor).")) {
        if (_hosting.isRunning()) {
            if (Config::cfg.overlay.enabled) {
                OverlayService::instance().start();
            }
            else {
                OverlayService::instance().stop();
            }
        }
        else if (!Config::cfg.overlay.enabled) {
            // If overlay is active for any reason while not hosting, close it.
            OverlayService::instance().stop();
        }
    }

    if (elCheckbox("Latency Limiter", _latencyLimiter, "Limit the maximum latency for guests.")) {
        if (_hosting.isRunning()) {
            _updated = true;
        }
    }

    if (elNumber("Latency Limit", _latencyLimit, 0, 64, "The maximum latency for guests (when latency limiter is enabled).")) {
        if (_hosting.isRunning()) {
            _updated = true;
        }
    }

    endBody();

    startFooter();

    static bool showPopup = false;

    if (!_hosting.isRunning()) {
        if (elBtn("Start Hosting")) {
            _popupTitle = "Start Hosting";
            _popupMessage = "Are you sure you want to start hosting?";
            Config::cfg.Save();
            savePreferences();
            showPopup = true;
            ImGui::OpenPopup(_popupTitle.c_str());
        }
    } else {
        if (isDirty()) {
            if (elBtn("Update Settings")) {
                if (validateSettings()) {
                    savePreferences();

                    _hosting.setHostConfig(_roomName, _gameID, _maxGuests, !Config::cfg.room.privateRoom, _secret);
                    _hosting.applyHostConfig();

                    if (!Config::cfg.room.privateRoom) {
                        Arcade::instance.createPost();
                    } else {
                        Arcade::instance.deletePost();
                    }

                    _updated = false;
                    updateSecretLink();
                }
            }
            ImGui::SameLine();
        }

        if (elBtnSecondary("Stop Hosting")) {
            _popupTitle = "Stop Hosting";
            _popupMessage = "Are you sure you want to stop hosting?";
            showPopup = true;
            ImGui::OpenPopup(_popupTitle.c_str());
        }
    }

    if (showPopup) {
        if (ConfirmPopupWidget::render(
            _popupTitle.c_str(),
            showPopup,
            _popupMessage
        ))
        {

            if (!_hosting.isRunning()) {
                // Start hosting
                if (validateSettings()) {
                    _hosting.setHostConfig("", _gameID, _maxGuests, false, _secret);
                    _hosting.applyHostConfig();
                    _hosting.startHosting();
                    if (_onHostRunningStatusCallback != nullptr) _onHostRunningStatusCallback(true);
                }
            } else {
                // Stop hosting
                _hosting.stopHosting();
                if (_onHostRunningStatusCallback != nullptr) _onHostRunningStatusCallback(false);
            }

        }
    }

    endFooter();
    endWidget();

    return true;

}

/// <summary>
/// Renders the audio settings panel.
/// </summary>
void HostSettingsWidget::renderAudio() {

    ImGui::Dummy(ImVec2(0.0f, 10.0f));

    if (!_hosting.isRunning() && _hosting.isReady())
    {
        _audioIn.captureAudio();
        _audioOut.captureAudio();
    }

    static int previousMicVolume, previousSpeakersVolume;
    static bool isVolumeChanged = false;
    previousMicVolume = _micVolume;
    previousSpeakersVolume = _speakersVolume;

    // ====================================================
    // Microphone
    // ====================================================
    static float micPreview, targetPreview;
    _micVolume = (int)(100.0f * _audioIn.volume);
    targetPreview = AudioTools::decibelToFloat(_audioIn.popPreviewDecibel());
    micPreview = lerp(micPreview, targetPreview, easing(targetPreview - micPreview));

    const static auto volumeReleaseCallback = [&]() {
        savePreferences();
    };

    if (AudioControlWidget::render("Microphone", &_micVolume, _audioIn.isEnabled, micPreview, AppIcons::micOn, AppIcons::micOff, volumeReleaseCallback))
    {
        _audioIn.isEnabled = !_audioIn.isEnabled;
        savePreferences();
    }
    _audioIn.volume = (float)_micVolume / 100.0f;

    // ====================================================
    // Speakers
    // ====================================================
    static float speakersPreview;
    _speakersVolume = (int)(100.0f *_audioOut.volume);
    targetPreview = AudioTools::decibelToFloat(_audioOut.popPreviewDecibel());
    speakersPreview = lerp(speakersPreview, targetPreview, easing(targetPreview - speakersPreview));
    if (AudioControlWidget::render("Speakers", &_speakersVolume, _audioOut.isEnabled, speakersPreview, AppIcons::speakersOn, AppIcons::speakersOff, volumeReleaseCallback))
    {
        _audioOut.isEnabled = !_audioOut.isEnabled;
        savePreferences();
    }
    _audioOut.volume = (float)_speakersVolume / 100.0f;

    ImGui::End();

}

/// <summary>
/// Saves the new preferences.
/// </summary>
void HostSettingsWidget::savePreferences() {
    Config::cfg.room.name = _roomName;
	Config::cfg.room.game = _gameName;
	Config::cfg.room.details = _description;
    Config::cfg.room.guestLimit = _maxGuests;
    Config::cfg.room.secret = _secret;
    Config::cfg.audio.micVolume = _micVolume;
    Config::cfg.audio.micEnabled = _audioIn.isEnabled;
    Config::cfg.audio.speakersVolume = _speakersVolume;
    Config::cfg.audio.speakersEnabled = _audioOut.isEnabled;
    Config::cfg.room.latencyLimit = _latencyLimiter;
    Config::cfg.room.latencyLimitThreshold = _latencyLimit;

    // Theme
    //string theme = _postThemes[_selectedTheme];
    //transform(theme.begin(), theme.end(), theme.begin(), ::tolower);
    //Config::cfg.room.theme = theme;

    // Artwork
    Config::cfg.room.artworkID = Arcade::instance.artworkID;

    Config::cfg.Save();
}

/// <summary>
/// Updates the secret link used when hosting room privately.
/// </summary>
void HostSettingsWidget::updateSecretLink() {

    try
    {
        const string parsecLink = string("https://parsec.gg/g/") + _hosting.getSession().hostPeerId + "/" + _secret + "/";
        if (Config::cfg.room.privateRoom) {
            strcpy_s(
                _shareLink,
                128,
                parsecLink.c_str()
            );
        } else {
            if (!Arcade::instance.credentials.username.empty()) {
                strcpy_s(
                    _shareLink,
                    128,
                    (string("https://soda-arcade.com/invite/") + Arcade::instance.credentials.username + "/room/").c_str()
                );
            } else {
                // Username can be populated slightly later than token/session updates.
                // Keep a valid link instead of rendering a malformed invite URL.
                strcpy_s(
                    _shareLink,
                    128,
                    parsecLink.c_str()
                );
            }
        }
    }
    catch (const std::exception&) {}

}

/// <summary>
/// If any settings have been changed once hosting is started,
/// then these settings are "dirty".
/// </summary>
bool HostSettingsWidget::isDirty() {
    if (_updated) {
        return true;
    } 
    return false;

    /*ParsecHostConfig cfg = _hosting.getHostConfig();

    if (
        _maxGuests != cfg.maxGuests ||
        _latencyLimiter != Config::cfg.room.latencyLimit ||
        _latencyLimit != Config::cfg.room.latencyLimitThreshold ||
        _hotseat != Config::cfg.hotseat.enabled ||
        _kioskMode != Config::cfg.kioskMode.enabled ||
        _selectedArtwork != Config::cfg.room.artworkID ||
        _gameName != Config::cfg.room.game ||
        _repThreshold != Config::cfg.room.repThreshold
    ) return true;

    return false;*/
}

/// <summary>
/// Helper lerp function.
/// </summary>
float HostSettingsWidget::lerp(float val1, float val2, float t) {
    return (1-t) * val1 + t * val2;
}

/// <summary>
/// Helper easing function.
/// </summary>
float HostSettingsWidget::easing(float t) {
    return 0.3f * max(abs(t), 0.4f);
}



