#include "../services/WebSocket.h"
#include "Config.h"
#include "../Hosting.h"
#include "../helpers/Countries.h"
extern Hosting g_hosting;
Config Config::cfg;

namespace {
	bool IsExtendedVKey(UINT vk) {
		switch (vk) {
		case VK_RCONTROL:
		case VK_RMENU:
		case VK_INSERT:
		case VK_DELETE:
		case VK_HOME:
		case VK_END:
		case VK_PRIOR:
		case VK_NEXT:
		case VK_LEFT:
		case VK_RIGHT:
		case VK_UP:
		case VK_DOWN:
		case VK_NUMLOCK:
		case VK_DIVIDE:
			return true;
		default:
			return false;
		}
	}

	std::string ResolveHotkeyName(int keyCode) {
		if (keyCode <= 0) {
			return "[Unknown Key]";
		}

		const UINT vk = static_cast<UINT>(keyCode);
		const UINT scanCode = MapVirtualKeyA(vk, MAPVK_VK_TO_VSC);
		LONG lParam = static_cast<LONG>(scanCode << 16);
		if (IsExtendedVKey(vk)) {
			lParam |= 1 << 24;
		}

		char keyName[128] = { 0 };
		if (GetKeyNameTextA(lParam, keyName, static_cast<int>(sizeof(keyName))) > 0) {
			return std::string(keyName);
		}

		Keymap keymap = Keymap();
		const std::string fallback = keymap.findKeyByValue(keyCode);
		if (fallback != "[Unknown Key]") {
			return fallback;
		}

		return "VK " + std::to_string(keyCode);
	}

	std::string TrimCommand(const std::string& command) {
		const size_t first = command.find_first_not_of(" \t\r\n");
		if (first == std::string::npos) {
			return "";
		}

		const size_t last = command.find_last_not_of(" \t\r\n");
		return command.substr(first, last - first + 1);
	}
}

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
			cfg.general.theme = setValue(cfg.general.theme, j["General"]["theme"].get<string>());
			cfg.general.saveLog = setValue(cfg.general.saveLog, j["General"]["saveLog"].get<bool>());
			cfg.general.flashWindow = setValue(cfg.general.flashWindow, j["General"]["flashWindow"].get<bool>());
			cfg.general.ipBan = setValue(cfg.general.ipBan, j["General"]["ipBan"].get<bool>());
			cfg.general.parsecLogs = setValue(cfg.general.parsecLogs, j["General"]["parsecLogs"].get<bool>());
			cfg.general.hotkeyBB = setValue(cfg.general.hotkeyBB, j["General"]["hotkeyBB"].get<bool>());
			cfg.general.hotkeyLock = setValue(cfg.general.hotkeyLock, j["General"]["hotkeyLock"].get<bool>());
			cfg.general.blockVPN = setValue(cfg.general.blockVPN, j["General"]["blockVPN"].get<bool>());
			cfg.general.devMode = setValue(cfg.general.devMode, j["General"]["devMode"].get<bool>());

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
			cfg.video.lanczos = setValue(cfg.video.lanczos, j["Video"]["lanczos"].get<bool>());
			cfg.video.captureMethod = setValue(cfg.video.captureMethod, j["Video"]["captureMethod"].get<unsigned int>());
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
			cfg.room.name = setValue(cfg.room.name, j["Room"]["name"].get<string>());
			cfg.room.game = setValue(cfg.room.game, j["Room"]["game"].get<string>());
			cfg.room.privateRoom = setValue(cfg.room.privateRoom, j["Room"]["privateRoom"].get<bool>());
			cfg.room.guestLimit = setValue(cfg.room.guestLimit, j["Room"]["guestLimit"].get<unsigned int>());
			cfg.room.isValid = setValue(cfg.room.isValid, j["Room"]["isValid"].get<bool>());
			cfg.room.secret = setValue(cfg.room.secret, j["Room"]["secret"].get<string>());
			cfg.room.streamUrl = setValue(cfg.room.streamUrl, j["Room"]["streamUrl"].get<string>());
			cfg.room.repThreshold = setValue(cfg.room.repThreshold, j["Room"]["repThreshold"].get<unsigned int>());
			cfg.room.latencyLimit = setValue(cfg.room.latencyLimit, j["Room"]["latencyLimit"].get<bool>());
			cfg.room.latencyLimitThreshold = setValue(cfg.room.latencyLimitThreshold, j["Room"]["latencyLimitThreshold"].get<unsigned int>());
			cfg.room.anonymous = setValue(cfg.room.anonymous, j["Room"]["anonymous"].get<bool>());

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
			if (j.contains("Chat") && j["Chat"].contains("ttsEnabled")) {
				cfg.chat.ttsEnabled = setValue(cfg.chat.ttsEnabled, j["Chat"]["ttsEnabled"].get<bool>());
			}

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
			cfg.widgets.stream = setValue(cfg.widgets.stream, j["Widgets"]["stream"].get<bool>());

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
			cfg.arcade.country = setValue(cfg.arcade.country, j["Arcade"]["country"].get<string>());
			cfg.arcade.showLogin = setValue(cfg.arcade.showLogin, j["Arcade"]["showLogin"].get<bool>());
			cfg.arcade.countryIndex = setValue(cfg.arcade.countryIndex, j["Arcade"]["countryIndex"].get<unsigned int>());

			// Socket
			cfg.socket.enabled = setValue(cfg.socket.enabled, j["Socket"]["enabled"].get<bool>());
			cfg.socket.port = setValue(cfg.socket.port, j["Socket"]["port"].get<unsigned int>());

			// Hotkeys
			cfg.hotkeys.keys.clear();
			cfg.hotkeys.enabled = setValue(cfg.hotkeys.enabled, j["Hotkeys"]["enabled"].get<bool>());
			if (j.contains("Hotkeys") && j["Hotkeys"].contains("keys") && j["Hotkeys"]["keys"].is_array()) {
				json hotkeys = j["Hotkeys"]["keys"];
				for (json::iterator it = hotkeys.begin(); it != hotkeys.end(); ++it) {
					Hotkey hotkey;
					hotkey.command = it.value().value("command", "");
					hotkey.key = it.value().value("key", 0);
					if (hotkey.command.empty() || hotkey.key == 0) {
						continue;
					}

					hotkey.keyName = it.value().value("keyName", "");
					if (hotkey.keyName.empty() || hotkey.keyName == "[Unknown Key]") {
						hotkey.keyName = ResolveHotkeyName(hotkey.key);
					}

					bool updatedExisting = false;
					for (Hotkey& existing : cfg.hotkeys.keys) {
						if (existing.key == hotkey.key) {
							existing.command = hotkey.command;
							existing.keyName = hotkey.keyName;
							updatedExisting = true;
							break;
						}
					}

					if (!updatedExisting) {
						cfg.hotkeys.keys.push_back(hotkey);
					}
				}
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

	// Keep arcade country code and index synchronized for callers that use either field.
	{
		Countries countries;
		int resolvedIndex = -1;

		for (size_t i = 0; i < countries.list.size(); ++i) {
			if (countries.list[i].first == cfg.arcade.country) {
				resolvedIndex = static_cast<int>(i);
				break;
			}
		}

		// Backward compatibility for configs that may store display names.
		if (resolvedIndex == -1) {
			for (size_t i = 0; i < countries.list.size(); ++i) {
				if (countries.list[i].second == cfg.arcade.country) {
					resolvedIndex = static_cast<int>(i);
					cfg.arcade.country = countries.list[i].first;
					break;
				}
			}
		}

		if (resolvedIndex == -1) {
			if (cfg.arcade.countryIndex >= 0 && cfg.arcade.countryIndex < static_cast<int>(countries.list.size())) {
				resolvedIndex = cfg.arcade.countryIndex;
				cfg.arcade.country = countries.list[resolvedIndex].first;
			}
			else {
				resolvedIndex = 0;
				for (size_t i = 0; i < countries.list.size(); ++i) {
					if (countries.list[i].first == "US") {
						resolvedIndex = static_cast<int>(i);
						break;
					}
				}
				cfg.arcade.country = countries.list[resolvedIndex].first;
			}
		}

		cfg.arcade.countryIndex = resolvedIndex;
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
		{"devMode", cfg.general.devMode}
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
		{"lanczos", cfg.video.lanczos},
		{"captureMethod", cfg.video.captureMethod},
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
		{"name", cfg.room.name},
		{"game", cfg.room.game},
		{"privateRoom", cfg.room.privateRoom},
		{"guestLimit", cfg.room.guestLimit},
		{"isValid", cfg.room.isValid},
		{"secret", cfg.room.secret},
		{"latencyLimit", cfg.room.latencyLimit},
		{"latencyLimitThreshold", cfg.room.latencyLimitThreshold},
		{"streamUrl", cfg.room.streamUrl},
		{"repThreshold", cfg.room.repThreshold},
		{"anonymous", cfg.room.anonymous}
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
		{"messageNotification", cfg.chat.messageNotification},
		{"ttsEnabled", cfg.chat.ttsEnabled}
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
		{"devTools", cfg.widgets.devTools},
		{"stream", cfg.widgets.stream}
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
		{"country", cfg.arcade.country},
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
	if (Config::cfg.mapHotkey) {
		return;
	}

	Config::cfg.pendingHotkeyCommand = TrimCommand(Config::cfg.pendingHotkeyCommand);
	if (Config::cfg.pendingHotkeyCommand.empty()) {
		Config::cfg.mapHotkey = false;
		return;
	}

	Config::cfg.mapHotkey = true;
	auto keyThread = [&]() {
		while (Config::cfg.mapHotkey) {
			bool keyHandled = false;
			for (int keyCode = 0; keyCode < 256; ++keyCode) {
				if (GetAsyncKeyState(keyCode) & 0x8000) {

					// If escape is pressed, cancel the hotkey mapping
					if (keyCode == VK_ESCAPE) {
						Config::cfg.pendingHotkeyCommand = "";
						Config::cfg.mapHotkey = false;
						keyHandled = true;
						break;
					}

					// Modifier keys not allowed
					if (keyCode == VK_SHIFT || keyCode == VK_LSHIFT || keyCode == VK_RSHIFT ||
						keyCode == VK_CONTROL || keyCode == VK_LCONTROL || keyCode == VK_RCONTROL ||
						keyCode == VK_MENU || keyCode == VK_LMENU || keyCode == VK_RMENU ||
						keyCode == VK_LWIN || keyCode == VK_RWIN) {
						continue;
					}

					// Mouse buttons not allowed
					if (keyCode == VK_LBUTTON || keyCode == VK_RBUTTON || keyCode == VK_MBUTTON ||
						keyCode == VK_XBUTTON1 || keyCode == VK_XBUTTON2) {
						continue;
					}

					Config::cfg.AddHotkey(Config::cfg.pendingHotkeyCommand, keyCode);
					Config::cfg.mapHotkey = false;
					keyHandled = true;
					while (GetAsyncKeyState(keyCode) & 0x8000) {
						Sleep(10);
					}
					break;

				}
			}
			if (keyHandled) {
				break;
			}
			Sleep(100);
		}
	};
	std::thread(keyThread).detach();

}

void Config::AddHotkey(string command, int key) {
	const std::string normalizedCommand = TrimCommand(command);
	if (normalizedCommand.empty() || key <= 0) {
		return;
	}

	for (Hotkey& existing : cfg.hotkeys.keys) {
		if (existing.key == key) {
			existing.command = normalizedCommand;
			existing.keyName = ResolveHotkeyName(key);
			Config::cfg.Save();
			return;
		}
	}

	Hotkey hotkey;
	hotkey.command = normalizedCommand;
	hotkey.key = key;
	hotkey.keyName = ResolveHotkeyName(key);

	if (g_hosting.mainWindow != nullptr) {
		const int hotkeyId = static_cast<int>(cfg.hotkeys.keys.size());
		if (!RegisterHotKey(g_hosting.mainWindow, hotkeyId, MOD_CONTROL | MOD_NOREPEAT, key)) {
			return;
		}
	}

	cfg.hotkeys.keys.push_back(hotkey);
	Config::cfg.Save();
}

void Config::RemoveHotkey(int index) {
	if (index < 0 || index >= static_cast<int>(Config::cfg.hotkeys.keys.size())) {
		return;
	}

	if (g_hosting.mainWindow != nullptr) {
		for (int i = 0; i < static_cast<int>(Config::cfg.hotkeys.keys.size()); i++) {
			UnregisterHotKey(g_hosting.mainWindow, i);
		}
	}

	Config::cfg.hotkeys.keys.erase(Config::cfg.hotkeys.keys.begin() + index);

	if (g_hosting.mainWindow != nullptr) {
		for (int i = 0; i < static_cast<int>(Config::cfg.hotkeys.keys.size()); i++) {
			RegisterHotKey(g_hosting.mainWindow, i, MOD_CONTROL | MOD_NOREPEAT, Config::cfg.hotkeys.keys[i].key);
		}
	}

	Config::cfg.Save();
}
