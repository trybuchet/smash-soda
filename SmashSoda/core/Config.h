#pragma once

#include <string>
#include <direct.h>
#include <iostream>
#include <windows.h>
#include <shlobj.h>
#include "matoya.h"
#include <nlohmann/json.hpp>
#include <vector>
#include "PathHelper.h"
#include "Keymap.h"

using json = nlohmann::json;

using namespace std;

class Config {
	
public:
	class Hotkey {
	public:
		string command = "";
		string keyName = "";
		int key = 0;
	};

	class General {
	public:
		string theme = "Ember";
		bool saveLog = false;
		bool flashWindow = true;
		bool ipBan = true;
		bool parsecLogs = false;
		bool hotkeyBB = true;
		bool hotkeyLock = true;
		bool blockVPN = false;
		bool devMode = false;
	};
	
	class Audio {
	public:
		unsigned int inputDevice = 0;
		unsigned int outputDevice = 0;
		unsigned int micFrequency = 44100;
		float micVolume = 80;
		bool micEnabled = false;
		unsigned int speakersFrequency = 44100;
		float speakersVolume = 50;
		bool speakersEnabled = true;
		bool sfxEnabled = true;
	};

	class Video {
	public:
		unsigned int monitor = 0;
		unsigned int adapter = 0;
		int windowX = 0;
		int windowY = 0;
		int windowW = 1280;
		int windowH = 720;
		unsigned int resolutionIndex = 0;
		unsigned int fps = 60;
		unsigned int bandwidth = 20;
	};

	class Input {
	public:
		bool autoIndex = false;
		bool disableKeyboard = false;
		bool disableGuideButton = true;
		unsigned int xboxPuppetCount = 4;
		unsigned int ds4PuppetCount = 0;
		bool lockedPads = false;
		bool lockedGamepadLeftTrigger = false;
		bool lockedGamepadRightTrigger = false;
		bool lockedGamepadLX = false;
		bool lockedGamepadLY = false;
		bool lockedGamepadRX = false;
		bool lockedGamepadRY = false;
		unsigned int lockedGamepadButtons = 0;
	};

	class Room {
	public:
		string title = "";
		string details = "";
		string theme = "default";
		string name = "";
		string game = "";
		bool privateRoom = false;
		unsigned int guestLimit = 1;
		int artworkID = -1;
		bool isValid = false;
		string secret = "";
		bool latencyLimit = false;
		unsigned int latencyLimitThreshold = 0;
		string streamUrl = "";
		unsigned int repThreshold = 70;
		bool anonymous = true;
	};
	
	class Chat {
	public:
		string discord = "";
		string chatbot = "ChatBot";
		string chatbotName = "";
		unsigned int muteTime = 5;
		bool autoMute = true;
		unsigned int autoMuteTime = 500;
		string welcomeMessage = "";
		bool bonkEnabled = true;
		bool hostBonkProof = false;
		bool messageNotification = false;
		bool ttsEnabled = false;
	};

	class Widgets {
	public:
		bool host = true;
		bool guests = true;
		bool gamepads = true;
		bool chat = true;
		bool log = false;
		bool settings = false;
		bool buttonLock = false;
		bool library = false;
		bool hotseat = false;
		bool masterOfPuppets = false;
		bool audio = false;
		bool video = false;
		bool overlay = false;
		bool keyMapper = false;
		bool devTools = false;
		bool stream = false;
	};

	class Hotseat {
	public:
		bool enabled = false;
		int playTime = 15;
		int resetTime = 30;
		int reminderInterval = 5;
	};
	
	class KioskMode {
	public:
		bool enabled = false;
	};

	class Overlay {
	public:
		class Chat {
		public:
			bool active = true;
			string position = "top left";
			bool showHistory = true;
		};
		
		class Gamepads {
		public:
			bool active = true;
			bool showHotseatDuration = true;
			string position = "bottom center";
		};

		class Guests {
		public:
			bool active = true;
			bool showLatency = true;
			string position = "top right";
		};
		
		string port = "9002";
		string theme = "";
		int monitor = 0;
		bool update = true;
		bool enabled = false;
		float opacity = 0.9f;
		
		Chat chat;
		Gamepads gamepads;
		Guests guests;
	};

	class Permissions {
	public:
		class PermissionGroup {
		public:
			bool useBB = false;
			bool useSFX = false;
			bool changeControls = false;

			PermissionGroup(bool useBB, bool useSFX, bool changeControls) {
				this->useBB = useBB;
				this->useSFX = useSFX;
				this->changeControls = changeControls;
			};
		};

		PermissionGroup guest = PermissionGroup(false, false, true);
		PermissionGroup vip = PermissionGroup(true, true, true);
		PermissionGroup moderator = PermissionGroup(true, true, true);
	};

	class Arcade {
	public:
		bool showLogin = true;
		string country = "US";
		int countryIndex = 220;
	};

	class Socket {
		public:
		bool enabled = true;
		int port = 9002;
	};

	class Hotkeys {

		public:
			bool enabled = true;
			vector<Hotkey> keys = {};
	};

	class Developer {
	public:
		bool useDevDomain = false;
		string devDomain = "";
		bool useStagingDomain = false;
		string stagingDomain = "";
		bool skipUpdateCheck = false;
	};

	General general;
	Audio audio;
	Video video;
	Input input;
	Room room;
	Chat chat;
	Widgets widgets;
	Hotseat hotseat;
	KioskMode kioskMode;
	Overlay overlay;
	Permissions permissions;
	Arcade arcade;
	Socket socket;
	Hotkeys hotkeys;
	Developer developer;

	// Supported resolutions
	struct Resolution {
		std::string label;
		int width;
		int height;

		Resolution(std::string lbl, int w, int h) : label(lbl), width(w), height(h) {}
	};

	// Parsec legacy SDK maxes out at ~2K resolution
	std::vector<Resolution> resolutions = {
		{"Native (auto)", 0, 0},
		{"2560x1440 (2K)", 2560, 1440},
		{"2560x1080 (UW)", 2560, 1080},
		{"1920x1200", 1920, 1200},
		{"1920x1080 (1080p)", 1920, 1080},
		{"1680x1050", 1680, 1050},
		{"1600x900", 1600, 900},
		{"1366x768", 1366, 768},
		{"1280x1024", 1280, 1024},
		{"1280x720 (720p)", 1280, 720},
		{"1024x768", 1024, 768},
		{"854x480 (480p)", 854, 480}
	};

	// Soda Arcade models
	class ArcadePost {
	public:
		string link = "";
		string peer_id = "";
		string secret = "";
		string slug = "";
		string game = "";
		string details = "";
		bool privateRoom = false;
		bool latencyLimit = 0;
		unsigned int guestLimit = 1;
		string theme = 0;
		int artwork = -1;
	};

	class Artwork {
	public:
		int id = 0;
		string title = "";
	};

	vector<Artwork> artwork = vector<Artwork>();

	// Singleton config
	static Config cfg;
	bool loaded = false;

	// Handles refreshing of room details on Parsec and Soda Arcade
	bool roomChanged = false;

	// Formats the custom chatbot name
	string chatbotName = "[ChatBot] ";

	// Overlay theme names loaded from the themes folder
	vector<string> overlayThemes = vector<string>();

	bool mapHotkey = false;
	string pendingHotkeyCommand = "";
	int pendingHotkey = -1;

	// Constructor
	Config() {

		// Generate a random secret for the room and post
		srand(time(NULL));
		string secret = "";
		for (int i = 0; i < 8; i++) {
			secret += (char)(rand() % 26 + 97);
		}
		cfg.room.secret = secret;

		// Initialize the chatbot name
		chatbotName = "[" + cfg.chat.chatbot + "] ";

		// Add a "None" option to the artwork list
		Artwork none;
		none.id = -1;
		none.title = "None";
		artwork.push_back(none);

	}

	void static Load();
	void Save();

	// Helper function to set a value in the config object
	template <typename T>
	static T setValue(const T& originalValue, const json& newValue) {
		if (!newValue.is_null()) {
			try {
				return newValue.get<T>();
			}
			catch (const json::type_error& e) {
				std::cerr << "Error: Type mismatch when setting config property" << std::endl;
			}
		}
		return originalValue;
	}

	void SetHotkey();
	void AddHotkey(string command, int key);
	void RemoveHotkey(int index);

private:
	void static LoadOverlayThemes();
};



