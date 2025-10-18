#include "../Modules/WebSocket.h"
#include "Config.h"
#include "../Hosting.h"
extern Hosting g_hosting;
Config Config::cfg;

/// <summary>
/// Loads the config file.
/// </summary>
void Config::Load() {

	// Get the config path
	string configPath = PathHelper::GetConfigPath() + "config.json";
	if (MTY_FileExists(configPath.c_str())) {

		try {

			size_t size;
			void* data = MTY_ReadFile(configPath.c_str(), &size);

			// Parse the json
			json j = json::parse((char*)data);

			// Set General properties
			cfg.general.theme = setValue(cfg.general.theme, j["General"]["theme"].get<unsigned int>());
			cfg.general.saveLog = setValue(cfg.general.saveLog, j["General"]["saveLog"].get<bool>());
			cfg.general.flashWindow = setValue(cfg.general.flashWindow, j["General"]["flashWindow"].get<bool>());
			cfg.general.ipBan = setValue(cfg.general.ipBan, j["General"]["ipBan"].get<bool>());
			cfg.general.parsecLogs = setValue(cfg.general.parsecLogs, j["General"]["parsecLogs"].get<bool>());
			cfg.general.hotkeyBB = setValue(cfg.general.hotkeyBB, j["General"]["hotkeyBB"].get<bool>());
			cfg.general.hotkeyLock = setValue(cfg.general.hotkeyLock, j["General"]["hotkeyLock"].get<bool>());
			cfg.general.blockVPN = setValue(cfg.general.blockVPN, j["General"]["blockVPN"].get<bool>());
			cfg.general.devMode = setValue(cfg.general.devMode, j["General"]["devMode"].get<bool>());
			cfg.general.autorun = setValue(cfg.general.devMode, j["General"]["autorun"].get<bool>());
			cfg.general.logShareLink = setValue(cfg.general.logShareLink, j["General"]["logShareLink"].get<bool>());

			// Set Audio properties
			cfg.audio.inputDevice = setValue(cfg.audio.inputDevice, j["Audio"]["inputDevice"].get<unsigned int>());
			cfg.audio.outputDevice = setValue(cfg.audio.outputDevice, j["Audio"]["outputDevice"].get<unsigned int>());
			cfg.audio.micFrequency = setValue(cfg.audio.micFrequency, j["Audio"]["micFrequency"].get<unsigned int>());
			cfg.audio.micVolume = setValue(cfg.audio.micVolume, j["Audio"]["micVolume"].get<float>());
			cfg.audio.micEnabled = setValue(cfg.audio.micEnabled, j["Audio"]["microphoneEnabled"].get<bool>());
			cfg.audio.speakersFrequency = setValue(cfg.audio.speakersFrequency, j["Audio"]["speakersFrequency"].get<unsigned int>());
			cfg.audio.speakersVolume = setValue(cfg.audio.speakersVolume, j["Audio"]["speakersVolume"].get<float>());
			cfg.audio.speakersEnabled = setValue(cfg.audio.speakersEnabled, j["Audio"]["speakersEnabled"].get<bool>());
			cfg.audio.sfxEnabled = setValue(cfg.audio.sfxEnabled, j["Audio"]["sfxEnabled"].get<bool>());

			// Set Video properties
			cfg.video.monitor = setValue(cfg.video.monitor, j["Video"]["monitor"].get<unsigned int>());
			cfg.video.adapter = setValue(cfg.video.adapter, j["Video"]["adapter"].get<unsigned int>());
			cfg.video.windowX = setValue(cfg.video.windowX, j["Video"]["windowX"].get<int>());
			cfg.video.windowY = setValue(cfg.video.windowY, j["Video"]["windowY"].get<int>());
			cfg.video.windowW = setValue(cfg.video.windowW, j["Video"]["windowW"].get<int>());
			cfg.video.windowH = setValue(cfg.video.windowH, j["Video"]["windowH"].get<int>());
			cfg.video.resolutionIndex = setValue(cfg.video.resolutionIndex, j["Video"]["resolutionIndex"].get<unsigned int>());
			cfg.video.fps = setValue(cfg.video.fps, j["Video"]["fps"].get<unsigned int>());
			cfg.video.bandwidth = setValue(cfg.video.bandwidth, j["Video"]["bandwidth"].get<unsigned int>());

			// Set Input properties
			cfg.input.autoIndex = setValue(cfg.input.autoIndex, j["Input"]["autoIndex"].get<bool>());
			cfg.input.disableKeyboard = setValue(cfg.input.disableKeyboard, j["Input"]["disableKeyboard"].get<bool>());
			cfg.input.disableGuideButton = setValue(cfg.input.disableGuideButton, j["Input"]["disableGuideButton"].get<bool>());
			cfg.input.xboxPuppetCount = setValue(cfg.input.xboxPuppetCount, j["Input"]["xboxPuppetCount"].get<unsigned int>());
			cfg.input.ds4PuppetCount = setValue(cfg.input.ds4PuppetCount, j["Input"]["ds4PuppetCount"].get<unsigned int>());
			cfg.input.lockedPads = setValue(cfg.input.lockedPads, j["Input"]["lockedPads"].get<bool>());
			cfg.input.lockedGamepadLeftTrigger = setValue(cfg.input.lockedGamepadLeftTrigger, j["Input"]["lockedGamepadLeftTrigger"].get<bool>());
			cfg.input.lockedGamepadRightTrigger = setValue(cfg.input.lockedGamepadRightTrigger, j["Input"]["lockedGamepadRightTrigger"].get<bool>());
			cfg.input.lockedGamepadLX = setValue(cfg.input.lockedGamepadLX, j["Input"]["lockedGamepadLX"].get<bool>());
			cfg.input.lockedGamepadLY = setValue(cfg.input.lockedGamepadLY, j["Input"]["lockedGamepadLY"].get<bool>());
			cfg.input.lockedGamepadRX = setValue(cfg.input.lockedGamepadRX, j["Input"]["lockedGamepadRX"].get<bool>());
			cfg.input.lockedGamepadRY = setValue(cfg.input.lockedGamepadRY, j["Input"]["lockedGamepadRY"].get<bool>());
			cfg.input.lockedGamepadButtons = setValue(cfg.input.lockedGamepadButtons, j["Input"]["lockedGamepadButtons"].get<unsigned int>());

			// Set Room properties
			cfg.room.title = setValue(cfg.room.title, j["Room"]["title"].get<string>());
			cfg.room.details = setValue(cfg.room.details, j["Room"]["details"].get<string>());
			cfg.room.theme = setValue(cfg.room.theme, j["Room"]["theme"].get<string>());
			cfg.room.game = setValue(cfg.room.game, j["Room"]["game"].get<string>());
			cfg.room.privateRoom = setValue(cfg.room.privateRoom, j["Room"]["privateRoom"].get<bool>());
			cfg.room.guestLimit = setValue(cfg.room.guestLimit, j["Room"]["guestLimit"].get<unsigned int>());
			cfg.room.isValid = setValue(cfg.room.isValid, j["Room"]["isValid"].get<bool>());
			cfg.room.secret = setValue(cfg.room.secret, j["Room"]["secret"].get<string>());
			cfg.room.streamUrl = setValue(cfg.room.streamUrl, j["Room"]["streamUrl"].get<string>());
			cfg.room.repThreshold = setValue(cfg.room.repThreshold, j["Room"]["repThreshold"].get<unsigned int>());
			cfg.room.latencyLimit = setValue(cfg.room.latencyLimit, j["Room"]["latencyLimit"].get<bool>());
			cfg.room.latencyLimitThreshold = setValue(cfg.room.latencyLimitThreshold, j["Room"]["latencyLimitThreshold"].get<unsigned int>());

			// Set Chat properties
			cfg.chat.discord = setValue(cfg.chat.discord, j["Chat"]["discord"].get<string>());
			cfg.chat.chatbot = setValue(cfg.chat.chatbot, j["Chat"]["chatbot"].get<string>());
			cfg.chat.chatbotName = setValue(cfg.chat.chatbotName, j["Chat"]["chatbotName"].get<string>());
			cfg.chat.muteTime = setValue(cfg.chat.muteTime, j["Chat"]["muteTime"].get<unsigned int>());
			cfg.chat.autoMute = setValue(cfg.chat.autoMute, j["Chat"]["autoMute"].get<bool>());
			cfg.chat.autoMuteTime = setValue(cfg.chat.autoMuteTime, j["Chat"]["autoMuteTime"].get<unsigned int>());
			cfg.chat.welcomeMessage = setValue(cfg.chat.welcomeMessage, j["Chat"]["welcomeMessage"].get<string>());
			cfg.chat.bonkEnabled = setValue(cfg.chat.bonkEnabled, j["Chat"]["bonkEnabled"].get<bool>());
			cfg.chat.hostBonkProof = setValue(cfg.chat.hostBonkProof, j["Chat"]["hostBonkProof"].get<bool>());
			cfg.chat.messageNotification = setValue(cfg.chat.messageNotification, j["Chat"]["messageNotification"].get<bool>());

			// Set Widgets properties
			cfg.widgets.host = setValue(cfg.widgets.host, j["Widgets"]["host"].get<bool>());
			cfg.widgets.guests = setValue(cfg.widgets.guests, j["Widgets"]["guests"].get<bool>());
			cfg.widgets.gamepads = setValue(cfg.widgets.gamepads, j["Widgets"]["gamepads"].get<bool>());
			cfg.widgets.chat = setValue(cfg.widgets.chat, j["Widgets"]["chat"].get<bool>());
			cfg.widgets.log = setValue(cfg.widgets.log, j["Widgets"]["log"].get<bool>());
			cfg.widgets.settings = setValue(cfg.widgets.settings, j["Widgets"]["settings"].get<bool>());
			cfg.widgets.buttonLock = setValue(cfg.widgets.buttonLock, j["Widgets"]["buttonLock"].get<bool>());
			cfg.widgets.library = setValue(cfg.widgets.library, j["Widgets"]["library"].get<bool>());
			cfg.widgets.hotseat = setValue(cfg.widgets.hotseat, j["Widgets"]["hotseat"].get<bool>());
			cfg.widgets.masterOfPuppets = setValue(cfg.widgets.masterOfPuppets, j["Widgets"]["masterOfPuppets"].get<bool>());
			cfg.widgets.audio = setValue(cfg.widgets.audio, j["Widgets"]["audio"].get<bool>());
			cfg.widgets.video = setValue(cfg.widgets.video, j["Widgets"]["video"].get<bool>());
			cfg.widgets.overlay = setValue(cfg.widgets.overlay, j["Widgets"]["overlay"].get<bool>());
			cfg.widgets.keyMapper = setValue(cfg.widgets.keyMapper, j["Widgets"]["keyMapper"].get<bool>());
			cfg.widgets.devTools = setValue(cfg.widgets.devTools, j["Widgets"]["devTools"].get<bool>());

			// Set Hotseat properties
			cfg.hotseat.enabled = setValue(cfg.hotseat.enabled, j["Hotseat"]["enabled"].get<bool>());
			cfg.hotseat.playTime = setValue(cfg.hotseat.playTime, j["Hotseat"]["playTime"].get<unsigned int>());
			cfg.hotseat.resetTime = setValue(cfg.hotseat.resetTime, j["Hotseat"]["resetTime"].get<unsigned int>());
			cfg.hotseat.reminderInterval = setValue(cfg.hotseat.reminderInterval, j["Hotseat"]["reminderInterval"].get<unsigned int>());

			// Set KioskMode properties
			cfg.kioskMode.enabled = false;

			// Set Overlay properties
			cfg.overlay.update = setValue(cfg.overlay.update, j["Overlay"]["update"].get<bool>());
			cfg.overlay.enabled = setValue(cfg.overlay.enabled, j["Overlay"]["enabled"].get<bool>());
			cfg.overlay.opacity = setValue(cfg.overlay.opacity, j["Overlay"]["opacity"].get<float>());
			cfg.overlay.theme = setValue(cfg.overlay.theme, j["Overlay"]["theme"].get<string>());
			cfg.overlay.monitor = setValue(cfg.overlay.monitor, j["Overlay"]["monitor"].get<unsigned int>());

			// Set Overlay::Chat properties
			cfg.overlay.chat.active = setValue(cfg.overlay.chat.active, j["Overlay"]["chat"]["active"].get<bool>());
			cfg.overlay.chat.position = setValue(cfg.overlay.chat.position, j["Overlay"]["chat"]["position"].get<string>());
			cfg.overlay.chat.showHistory = setValue(cfg.overlay.chat.showHistory, j["Overlay"]["chat"]["showHistory"].get<bool>());

			// Set Overlay::Gamepads properties
			cfg.overlay.gamepads.active = setValue(cfg.overlay.gamepads.active, j["Overlay"]["gamepads"]["active"].get<bool>());
			cfg.overlay.gamepads.position = setValue(cfg.overlay.gamepads.position, j["Overlay"]["gamepads"]["position"].get<string>());
			cfg.overlay.gamepads.showHotseatDuration = setValue(cfg.overlay.gamepads.showHotseatDuration, j["Overlay"]["gamepads"]["showHotseatDuration"].get<bool>());

			// Set Overlay::Guests properties
			cfg.overlay.guests.active = setValue(cfg.overlay.guests.active, j["Overlay"]["guests"]["active"].get<bool>());
			cfg.overlay.guests.position = setValue(cfg.overlay.guests.position, j["Overlay"]["guests"]["position"].get<string>());
			cfg.overlay.guests.showLatency = setValue(cfg.overlay.guests.showLatency, j["Overlay"]["guests"]["showLatency"].get<bool>());

			// Set Permissions properties
			cfg.permissions.guest.useBB = setValue(cfg.permissions.guest.useBB, j["Permissions"]["guest"]["useBB"].get<bool>());
			cfg.permissions.guest.useSFX = setValue(cfg.permissions.guest.useSFX, j["Permissions"]["guest"]["useSFX"].get<bool>());
			cfg.permissions.guest.changeControls = setValue(cfg.permissions.guest.changeControls, j["Permissions"]["guest"]["changeControls"].get<bool>());
			cfg.permissions.vip.useBB = setValue(cfg.permissions.vip.useBB, j["Permissions"]["vip"]["useBB"].get<bool>());
			cfg.permissions.vip.useSFX = setValue(cfg.permissions.vip.useSFX, j["Permissions"]["vip"]["useSFX"].get<bool>());
			cfg.permissions.vip.changeControls = setValue(cfg.permissions.vip.changeControls, j["Permissions"]["vip"]["changeControls"].get<bool>());
			cfg.permissions.moderator.useBB = setValue(cfg.permissions.moderator.useBB, j["Permissions"]["moderator"]["useBB"].get<bool>());
			cfg.permissions.moderator.useSFX = setValue(cfg.permissions.moderator.useSFX, j["Permissions"]["moderator"]["useSFX"].get<bool>());
			cfg.permissions.moderator.changeControls = setValue(cfg.permissions.moderator.changeControls, j["Permissions"]["moderator"]["changeControls"].get<bool>());

			// Set Arcade properties
			cfg.arcade.showLogin = setValue(cfg.arcade.showLogin, j["Arcade"]["showLogin"].get<bool>());
			cfg.arcade.countryIndex = setValue(cfg.arcade.countryIndex, j["Arcade"]["countryIndex"].get<unsigned int>());

			// Socket
			cfg.socket.enabled = setValue(cfg.socket.enabled, j["Socket"]["enabled"].get<bool>());
			cfg.socket.port = setValue(cfg.socket.port, j["Socket"]["port"].get<unsigned int>());

			// Hotkeys
			Keymap keymap = Keymap();
			cfg.hotkeys.enabled = setValue(cfg.hotkeys.enabled, j["Hotkeys"]["enabled"].get<bool>());
			json hotkeys = j["Hotkeys"]["keys"];
			for (json::iterator it = hotkeys.begin(); it != hotkeys.end(); ++it) {
				Hotkey hotkey;
				hotkey.command = it.value()["command"].get<string>();
				hotkey.key = it.value()["key"].get<int>();
				hotkey.keyName = keymap.findKeyByValue(hotkey.key);
				cfg.hotkeys.keys.push_back(hotkey);
			}

			// Developer
			cfg.developer.useDevDomain = setValue(cfg.developer.useDevDomain, j["Developer"]["useDevDomain"].get<bool>());
			cfg.developer.devDomain = setValue(cfg.developer.devDomain, j["Developer"]["devDomain"].get<string>());
			cfg.developer.useStagingDomain = setValue(cfg.developer.useStagingDomain, j["Developer"]["useStagingDomain"].get<bool>());
			cfg.developer.stagingDomain = setValue(cfg.developer.stagingDomain, j["Developer"]["stagingDomain"].get<string>());
			cfg.developer.skipUpdateCheck = setValue(cfg.developer.skipUpdateCheck, j["Developer"]["skipUpdateCheck"].get<bool>());

		} catch (json::exception &e) {
			// Handle exception
		}

		// Load custom overlay themes
		LoadOverlayThemes();

	}

}

/// <summary>
/// Saves the config file.
/// </summary>
void Config::Save() {

	json j;

	// General
	j["General"] = {
		{"theme", cfg.general.theme},
		{"saveLog", cfg.general.saveLog},
		{"flashWindow", cfg.general.flashWindow},
		{"ipBan", cfg.general.ipBan},
		{"parsecLogs", cfg.general.parsecLogs},
		{"hotkeyBB", cfg.general.hotkeyBB},
		{"hotkeyLock", cfg.general.hotkeyLock},
		{"blockVPN", cfg.general.blockVPN},
		{"devMode", cfg.general.devMode},
		{"autorun", cfg.general.autorun},
		{"logShareLink", cfg.general.logShareLink}
	};

	// Audio
	j["Audio"] = {
		{"inputDevice", cfg.audio.inputDevice},
		{"outputDevice", cfg.audio.outputDevice},
		{"micFrequency", cfg.audio.micFrequency},
		{"micVolume", cfg.audio.micVolume},
		{"microphoneEnabled", cfg.audio.micEnabled},
		{"speakersFrequency", cfg.audio.speakersFrequency},
		{"speakersVolume", cfg.audio.speakersVolume},
		{"speakersEnabled", cfg.audio.speakersEnabled},
		{"sfxEnabled", cfg.audio.sfxEnabled}
	};

	// Video
	j["Video"] = {
		{"monitor", cfg.video.monitor},
		{"adapter", cfg.video.adapter},
		{"windowX", cfg.video.windowX},
		{"windowY", cfg.video.windowY},
		{"windowW", cfg.video.windowW},
		{"windowH", cfg.video.windowH},
		{"resolutionIndex", cfg.video.resolutionIndex},
		{"fps", cfg.video.fps},
		{"bandwidth", cfg.video.bandwidth}
	};

	// Input
	j["Input"] = {
		{"autoIndex", cfg.input.autoIndex},
		{"disableKeyboard", cfg.input.disableKeyboard},
		{"disableGuideButton", cfg.input.disableGuideButton},
		{"xboxPuppetCount", cfg.input.xboxPuppetCount},
		{"ds4PuppetCount", cfg.input.ds4PuppetCount},
		{"lockedPads", cfg.input.lockedPads},
		{"lockedGamepadLeftTrigger", cfg.input.lockedGamepadLeftTrigger},
		{"lockedGamepadRightTrigger", cfg.input.lockedGamepadRightTrigger},
		{"lockedGamepadLX", cfg.input.lockedGamepadLX},
		{"lockedGamepadLY", cfg.input.lockedGamepadLY},
		{"lockedGamepadRX", cfg.input.lockedGamepadRX},
		{"lockedGamepadRY", cfg.input.lockedGamepadRY},
		{"lockedGamepadButtons", cfg.input.lockedGamepadButtons}
	};

	// Room
	j["Room"] = {
		{"title", cfg.room.title},
		{"details", cfg.room.details},
		{"theme", cfg.room.theme},
		{"game", cfg.room.game},
		{"privateRoom", cfg.room.privateRoom},
		{"guestLimit", cfg.room.guestLimit},
		{"isValid", cfg.room.isValid},
		{"secret", cfg.room.secret},
		{"latencyLimit", cfg.room.latencyLimit},
		{"latencyLimitThreshold", cfg.room.latencyLimitThreshold},
		{"streamUrl", cfg.room.streamUrl},
		{"repThreshold", cfg.room.repThreshold}
	};

	// Chat
	j["Chat"] = {
		{"discord", cfg.chat.discord},
		{"chatbot", cfg.chat.chatbot},
		{"chatbotName", cfg.chat.chatbotName},
		{"muteTime", cfg.chat.muteTime},
		{"autoMute", cfg.chat.autoMute},
		{"autoMuteTime", cfg.chat.autoMuteTime},
		{"welcomeMessage", cfg.chat.welcomeMessage},
		{"bonkEnabled", cfg.chat.bonkEnabled},
		{"hostBonkProof", cfg.chat.hostBonkProof},
		{"messageNotification", cfg.chat.messageNotification}
	};

	// Widgets
	j["Widgets"] = {
		{"host", cfg.widgets.host},
		{"guests", cfg.widgets.guests},
		{"gamepads", cfg.widgets.gamepads},
		{"chat", cfg.widgets.chat},
		{"log", cfg.widgets.log},
		{"settings", cfg.widgets.settings},
		{"buttonLock", cfg.widgets.buttonLock},
		{"library", cfg.widgets.library},
		{"hotseat", cfg.widgets.hotseat},
		{"masterOfPuppets", cfg.widgets.masterOfPuppets},
		{"audio", cfg.widgets.audio},
		{"video", cfg.widgets.video},
		{"overlay", cfg.widgets.overlay},
		{"keyMapper", cfg.widgets.keyMapper},
		{"devTools", cfg.widgets.devTools}
	};

	// Hotseat
	j["Hotseat"] = {
		{"enabled", cfg.hotseat.enabled},
		{"playTime", cfg.hotseat.playTime},
		{"resetTime", cfg.hotseat.resetTime},
		{"reminderInterval", cfg.hotseat.reminderInterval}
	};

	// KioskMode
	j["KioskMode"] = {
		{"enabled", cfg.kioskMode.enabled}
	};

	// Overlay
	j["Overlay"] = {
		{"update", cfg.overlay.update},
		{"enabled", cfg.overlay.enabled},
		{"opacity", cfg.overlay.opacity},
		{"theme", cfg.overlay.theme},
		{"monitor", cfg.overlay.monitor},
		{"chat", {
			{"active", cfg.overlay.chat.active},
			{"position", cfg.overlay.chat.position},
			{"showHistory", cfg.overlay.chat.showHistory}
		}},
		{"gamepads", {
			{"active", cfg.overlay.gamepads.active},
			{"position", cfg.overlay.gamepads.position},
			{"showHotseatDuration", cfg.overlay.gamepads.showHotseatDuration}
		}},
		{"guests", {
			{"active", cfg.overlay.guests.active},
			{"position", cfg.overlay.guests.position},
			{"showLatency", cfg.overlay.guests.showLatency}
		}}
	};

	// Permissions
	j["Permissions"] = {
		{"guest", {
			{"useBB", cfg.permissions.guest.useBB},
			{"useSFX", cfg.permissions.guest.useSFX},
			{"changeControls", cfg.permissions.guest.changeControls}
		}},
		{"vip", {
			{"useBB", cfg.permissions.vip.useBB},
			{"useSFX", cfg.permissions.vip.useSFX},
			{"changeControls", cfg.permissions.vip.changeControls}
		}},
		{"moderator", {
			{"useBB", cfg.permissions.moderator.useBB},
			{"useSFX", cfg.permissions.moderator.useSFX},
			{"changeControls", cfg.permissions.moderator.changeControls}
		}}
	};

	// Arcade
	j["Arcade"] = {
		{"showLogin", cfg.arcade.showLogin},
		{"countryIndex", cfg.arcade.countryIndex}
	};

	// Socket
	j["Socket"] = {
		{"enabled", cfg.socket.enabled},
		{"port", cfg.socket.port}
	};

	// Hotkeys
	vector <json> hotkeys;
	for (Hotkey hotkey : cfg.hotkeys.keys) {
		json hotkeyJson;
		hotkeyJson["command"] = hotkey.command;
		hotkeyJson["key"] = hotkey.key;
		hotkeyJson["keyName"] = hotkey.keyName;
		hotkeys.push_back(hotkeyJson);
	}

	j["Hotkeys"] = {
		{"enabled", cfg.hotkeys.enabled},
		{"keys", hotkeys}
	};

	// Developer
	j["Developer"] = {
		{"useDevDomain", cfg.developer.useDevDomain},
		{"devDomain", cfg.developer.devDomain},
		{"useStagingDomain", cfg.developer.useStagingDomain},
		{"stagingDomain", cfg.developer.stagingDomain},
		{"skipUpdateCheck", cfg.developer.skipUpdateCheck}
	};

	// Save the file
	string configPath = PathHelper::GetConfigPath();
	if (configPath != "") {

		// Filepath
		string filePath = configPath + "config.json";

		string configString = j.dump(4);
		bool success = MTY_WriteTextFile(filePath.c_str(), "%s", configString.c_str());

		// Send over websocket
		if (WebSocket::instance.isRunning()) {
			WebSocket::instance.sendMessageToAll("app:config");
		}

	}

}

// This function reads the overlay themes from the themes folder,
// then adds each filename to the overlayThemes vector.
void Config::LoadOverlayThemes() {

	// Get themes path
	string themesPath = PathHelper::GetCurrentPath() + "\\overlay\\themes";

	// Get all files in the directory
	vector<string> files = PathHelper::GetFilenames(themesPath.c_str(), false);

	// If there are files abort
	if (files.size() == 0) {
		return;
	}

	// For each file
	for (string file : files) {
		cfg.overlayThemes.push_back(file);
	}

}

/// <summary>
/// Map a hotkey to a command.
/// </summary>
void Config::SetHotkey() {
	Config::cfg.mapHotkey = true;
	auto keyThread = [&]() {
		while (Config::cfg.mapHotkey) {
			for (int keyCode = 0; keyCode < 256; ++keyCode) {
				if (GetAsyncKeyState(keyCode) & 0x8000) {

					// If escape is pressed, cancel the hotkey mapping
					if (keyCode == VK_ESCAPE) {
						Config::cfg.pendingHotkeyCommand = "";
						Config::cfg.mapHotkey = false;
						break;
					}

					// Modifier keys not allowed
					if (keyCode == VK_SHIFT || keyCode == VK_CONTROL || keyCode == VK_MENU) {
						break;
					}

					// Mouse buttons not allowed
					if (keyCode == VK_LBUTTON || keyCode == VK_RBUTTON || keyCode == VK_MBUTTON) {
						break;
					}

					Config::cfg.AddHotkey(Config::cfg.pendingHotkeyCommand, keyCode);
					Config::cfg.mapHotkey = false;

				}
			}
			Sleep(100);
		}
	};
	std::thread(keyThread).detach();

}

void Config::AddHotkey(string command, int key) {
	Hotkey hotkey;
	hotkey.command = command;
	hotkey.key = key;
	
	Keymap keymap = Keymap();
	hotkey.keyName = keymap.findKeyByValue(key);

	cfg.hotkeys.keys.push_back(hotkey);
	RegisterHotKey(g_hosting.mainWindow, cfg.hotkeys.keys.size()-1, MOD_CONTROL|MOD_NOREPEAT, key);
	Config::cfg.Save();
}

void Config::RemoveHotkey(int index) {
	for (int i = 0; i < Config::cfg.hotkeys.keys.size(); i++) {
		if (i == index) {
			UnregisterHotKey(g_hosting.mainWindow, i);
			Config::cfg.hotkeys.keys.erase(Config::cfg.hotkeys.keys.begin() + i);
			Config::cfg.Save();
			break;
		}
	}
}