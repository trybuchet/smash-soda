#include "Cache.h"
#include <thread>
#include <chrono>
#include <algorithm>
#include <cctype>
#include <Windows.h>
#include "Config.h"

// These are the sort of issues Parsec introduced when
// they forbid me from providing builds... ¬_¬
string Cache::_key = "SmashSodaKey***";
string Cache::_nonce = "SmashSoda**";

// Global cache object
Cache Cache::cache = Cache();
namespace {
	constexpr uint32_t DEFAULT_SFX_COOLDOWN = 5;
	string StripFileExtension(const string& fileName) {
		const size_t slashPos = fileName.find_last_of("/\\");
		const size_t startPos = (slashPos == string::npos) ? 0 : slashPos + 1;
		const size_t dotPos = fileName.find_last_of('.');
		if (dotPos == string::npos || dotPos <= startPos) {
			return fileName.substr(startPos);
		}
		return fileName.substr(startPos, dotPos - startPos);
	}
}

/**
 * @brief Cache constructor
 * Initializes the cache object.
 */
Cache::Cache() {

	// Set the version
	version = "7.0.0";

    // Load verified users
    vector<GuestData> verified = VerifiedList::LoadFromFile();
    verifiedList = VerifiedList(verified);

    // Load banned users
    vector<GuestData> banned = BanList::LoadFromFile();
    banList = BanList(banned);

    // Load moderators
    vector<GuestData> mods = ModList::LoadFromFile();
    modList = ModList(mods);

    // Load game data
    vector<GameData> games = GameDataList::LoadFromFile();
    gameList = GameDataList(games);

    // Load VIP users
    vector<GuestData> vips = VIPList::LoadFromFile();
    vipList = VIPList(vips);

	// Sync and load SFX from AppData
	reloadSfxList();

    // Load banned IP addresses from file
    LoadBannedIpAddresses();

    // Load tiers
	tierList.loadTiers();

	// Check for updates
	checkForUpdates();

	// Get the VPN list
	getVPNList();

}

void Cache::reloadSfxList() {
	const string configPath = PathHelper::GetConfigPath();
	if (configPath.empty()) {
		return;
	}

	syncSfxConfig();
	sfxList.init((configPath + "sfx.json").c_str());
}

vector<string> Cache::listCustomSfxFiles() const {
	vector<string> files;
	auto hasAudioExtension = [](const string& fileName) {
		size_t dotPos = fileName.find_last_of('.');
		if (dotPos == string::npos) {
			return false;
		}

		string ext = fileName.substr(dotPos);
		transform(ext.begin(), ext.end(), ext.begin(), [](unsigned char c) {
			return static_cast<char>(tolower(c));
		});

		return ext == ".wav" || ext == ".mp3" || ext == ".ogg" || ext == ".flac" || ext == ".m4a";
	};

	WIN32_FIND_DATAA findData;
	HANDLE hFind = FindFirstFileA("./SFX/custom/*", &findData);
	if (hFind == INVALID_HANDLE_VALUE) {
		return files;
	}

	do {
		if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			continue;
		}

		string fileName = findData.cFileName;
		if (!hasAudioExtension(fileName)) {
			continue;
		}

		files.push_back(fileName);
	} while (FindNextFileA(hFind, &findData));

	FindClose(hFind);
	sort(files.begin(), files.end());
	return files;
}

void Cache::syncSfxConfig() {
	const string configPath = PathHelper::GetConfigPath();
	if (configPath.empty()) {
		return;
	}

	const string sfxConfigPath = configPath + "sfx.json";
	unordered_map<string, uint32_t> cooldownByTag;

	if (MTY_FileExists(sfxConfigPath.c_str())) {
		try {
			size_t size = 0;
			void* data = MTY_ReadFile(sfxConfigPath.c_str(), &size);
			if (data != nullptr && size > 0) {
				string text(static_cast<char*>(data), size);
				json existing = json::parse(text);

				if (existing.contains("sfx") && existing["sfx"].is_array()) {
					for (const auto& entry : existing["sfx"]) {
						if (!entry.is_object()) {
							continue;
						}

							string tag = entry.value("tag", "");
							if (tag.empty()) {
								tag = entry.value("path", "");
							}
							if (tag.empty()) {
								continue;
							}

							const uint32_t cooldown = entry.value("cooldown", DEFAULT_SFX_COOLDOWN);
							const string path = entry.value("path", "");
							cooldownByTag[tag] = cooldown;
							cooldownByTag[StripFileExtension(tag)] = cooldown;
							if (!path.empty()) {
								cooldownByTag[path] = cooldown;
								cooldownByTag[StripFileExtension(path)] = cooldown;
							}
						}
					}
				}
			if (data != nullptr) {
				MTY_Free(data);
			}
		}
		catch (const std::exception&) {}
	}

	const vector<string> files = listCustomSfxFiles();

	json out;
	out["sfx"] = json::array();
	for (const auto& fileName : files) {
		const string tag = StripFileExtension(fileName);
		json entry;
		entry["path"] = fileName;
		entry["tag"] = tag;
		const auto tagIt = cooldownByTag.find(tag);
		const auto fileIt = cooldownByTag.find(fileName);
		entry["cooldown"] = tagIt != cooldownByTag.end()
			? tagIt->second
			: (fileIt != cooldownByTag.end() ? fileIt->second : DEFAULT_SFX_COOLDOWN);
		out["sfx"].push_back(entry);
	}

	const string outText = out.dump(4);
	MTY_WriteTextFile(sfxConfigPath.c_str(), "%s", outText.c_str());
}

SessionCache Cache::loadSessionCache() {
    SessionCache result = SessionCache();

    string dirPath = PathHelper::GetConfigPath();
    if (!dirPath.empty()) {
        string filepath = dirPath + "session.json";

        if (MTY_FileExists(filepath.c_str())) {
            size_t size;
            const char* encryptedContent;
            encryptedContent = (char*)MTY_ReadFile(filepath.c_str(), &size);

            char originalText[256];
            MTY_AESGCM* ctx = MTY_AESGCMCreate(_key.c_str());
            char tag[16] = "SmashSodaTag**";

            MTY_AESGCMDecrypt(ctx, _nonce.c_str(), encryptedContent, size, tag, (void*)originalText);
            MTY_JSON* json = MTY_JSONParse(originalText);
            if (json != nullptr)
            {
                char sessionID[128];
                char peerID[64];
                uint32_t type = (uint32_t)SessionCache::SessionType::THIRD;
                uint32_t expiry = 0, start = 0;

                bool success =
                    MTY_JSONObjGetString(json, "sessionID", sessionID, 128)
                    && MTY_JSONObjGetString(json, "peerID", peerID, 64)
                    && MTY_JSONObjGetUInt(json, "type", &type)
                    && MTY_JSONObjGetUInt(json, "start", &start)
                    && MTY_JSONObjGetUInt(json, "expiry", &expiry)
                    ;

                if (success)
                {
                    result.isValid = true;
                    result.sessionID = sessionID;
                    result.peerID = peerID;
                    result.type = (SessionCache::SessionType)type;
                    result.start = start;
                    result.expiry = expiry;
                }

                MTY_JSONDestroy(&json);
            }

            MTY_AESGCMDestroy(&ctx);
        }
    }

    return result;
}

bool Cache::saveSessionCache(SessionCache sessionCache) {
    bool result = false;
    string dirPath = PathHelper::GetConfigPath();
    if (!dirPath.empty()) {
        string filepath = dirPath + "session.json";

        if (sessionCache.isValid)
        {
            MTY_JSON* json = MTY_JSONObjCreate();
            MTY_JSONObjSetString(json, "sessionID", sessionCache.sessionID.c_str());
            MTY_JSONObjSetString(json, "peerID", sessionCache.peerID.c_str());
            MTY_JSONObjSetUInt(json, "type", (uint32_t)sessionCache.type);
            MTY_JSONObjSetUInt(json, "start", sessionCache.start);
            MTY_JSONObjSetUInt(json, "expiry", sessionCache.expiry);

            string jsonStr = MTY_JSONSerialize(json);
            char encryptedJson[256];
            char tag[16] = "ParsecSodaTag**";
            MTY_AESGCM* ctx = MTY_AESGCMCreate(_key.c_str());
            if (MTY_AESGCMEncrypt(ctx, _nonce.c_str(), jsonStr.c_str(), jsonStr.size(), tag, encryptedJson))
            {
                if (MTY_WriteFile(filepath.c_str(), encryptedJson, jsonStr.size()))
                {
                    result = true;
                }
            }

            MTY_AESGCMDestroy(&ctx);
            MTY_JSONDestroy(&json);
        }
    }

    return result;
}

bool Cache::deleteSessionCache() {
	string dirPath = PathHelper::GetConfigPath();
    if (!dirPath.empty()) {
		string filepath = dirPath + "session.json";
		return MTY_DeleteFile(filepath.c_str());
	}

	return false;
}

/**
 * Check for Smash Soda updates.
 */
bool Cache::checkForUpdates() {
	// Respect developer setting to skip update checks
	if (Config::cfg.developer.skipUpdateCheck) {
		return false;
	}
	
	string data = "";
	size_t bodySize = sizeof(char) * data.length();

	// Prepare the response
	uint16_t _status = 0;
	void* response = nullptr;
	size_t responseSize = 0;

	// Send the request
	string headers = "Content-Type: application/json\r\n";
	headers += "Content-Length: " + to_string(bodySize) + "\r\n";

	string domain = "raw.githubusercontent.com";
	// Add a cache-buster to avoid CDN stale responses
	const auto ts = std::chrono::duration_cast<std::chrono::seconds>(
		std::chrono::system_clock::now().time_since_epoch()
	).count();
	string path = "/trybuchet/smash-soda-registry/refs/heads/main/app.json?ts=" + std::to_string(ts);
	string method = "GET";

	const bool success = MTY_HttpRequest(
		domain.c_str(), 0, true, method.c_str(), path.c_str(),
		headers.c_str(),
		data.c_str(), bodySize, 20000,
		&response, &responseSize, &_status
	);

	const char* responseStr = (const char*)response;
	if (responseSize > 0 && _status == 200 && responseStr != nullptr) {
		string responseString = string(responseStr, responseSize);

		try {
			json j = json::parse(responseString);

			// Set update properties (safe defaults if keys are missing)
			update.version = j.value("version", version);
			update.notes = j.value("notes", "");
			update.overlay = j.value("overlay", "");

			// Add banned users if not already banned
			globalBans.clear();
			if (j.contains("banned") && j["banned"].is_array()) {
				for (auto& bannedId : j["banned"]) {
					if (bannedId.is_number_unsigned() && !isGlobalBanned(bannedId.get<uint32_t>())) {
						addGlobalBan(bannedId.get<uint32_t>());
					}
				}
			}

			// Add the SODA COPS!!!
			sodaCops.clear();
			if (j.contains("cops") && j["cops"].is_array()) {
				for (auto& copId : j["cops"]) {
					if (copId.is_number_unsigned() && !isSodaCop(copId.get<uint32_t>())) {
						addSodaCop(copId.get<uint32_t>());
					}
				}
			}

			// Has the version changed?
			return version != update.version;

		} catch (json::exception &) {
			return false;
		}
	}

	return false;

}

/**
 * @brief Ban an IP address
 * @param ip The IP address to ban
 */
void Cache::banIPAddress(std::string ip) {

    // If the ip address is already banned do nothing
	for (auto& bannedIP : bannedIPs) {
		if (bannedIP == ip) {
			return;
		}
	}

	// Add the ip address to the list
	bannedIPs.push_back(ip);

	// Save the banned ip addresses
	json j;
	for (auto& ip : bannedIPs) {
		j.push_back(ip);
	}

	// Save the file
	string ipPath = PathHelper::GetConfigPath() + "banned_ip.json";
	string ipString = j.dump(4);
	bool success = MTY_WriteTextFile(ipPath.c_str(), "%s", ipString.c_str());

}

/**
 * @brief Unban an IP address
 * @param ip The IP address to unban
 */
void Cache::unbanIPAddress(std::string ip) {
    for (auto& bannedIP : bannedIPs) {
		if (bannedIP == ip) {
			bannedIPs.erase(std::remove(bannedIPs.begin(), bannedIPs.end(), ip), bannedIPs.end());
		}
	}

	// Save the banned ip addresses
	json j;
	for (auto& ip : bannedIPs) {
		j.push_back(ip);
	}

	// Save the file
	string ipPath = PathHelper::GetConfigPath() + "banned_ip.json";
	string ipString = j.dump(4);
	bool success = MTY_WriteTextFile(ipPath.c_str(), "%s", ipString.c_str());
}

/**
 * @brief Remove last IP address from ban list.
 */
void Cache::unbanLastIPAddress() {
	// Was the last IP address banned?
	if (isBannedIPAddress(lastIpAddress)) {
		// Unban the IP address
		unbanIPAddress(lastIpAddress);
	}
}

/**
 * @brief Check if an IP address is banned
 * @param ip The IP address to check
 * @return True if the IP address is banned, false otherwise
 */
bool Cache::isBannedIPAddress(std::string ip) {
    return std::find(bannedIPs.begin(), bannedIPs.end(), ip) != bannedIPs.end();
}

/**
 * @brief Get the IP address of a user
 * @param userId The user ID to get the IP address of
 * @return The IP address of the user
 */
std::string Cache::getUserIpAddress(uint32_t userId) {
    return userIpMap[userId];
}

/**
 * @brief Load the banned IP addresses from file
 */
void Cache::LoadBannedIpAddresses() {

    // Load banned IP addresses
	string ipPath = PathHelper::GetConfigPath() + "banned_ip.json";
	if (MTY_FileExists(ipPath.c_str())) {

		try {

			size_t size;
			void* data = MTY_ReadFile(ipPath.c_str(), &size);

			// Parse the json
			json j = json::parse((char*)data);

			// Set the banned IP addresses
			for (auto& element : j) {
				bannedIPs.push_back(element.get<string>());
			}

		} catch (json::exception &e) {
			// Handle exception
		}

	}

}

// Get the list of VPN IP addresses
void Cache::getVPNList() {
	string data = "";
	size_t bodySize = sizeof(char) * data.length();

	// Prepare the response
	uint16_t _status = 0;
	void* response = nullptr;
	size_t responseSize = 0;

	// Send the request
	string headers = "Content-Type: application/json\r\n";
	headers += "Content-Length: " + to_string(bodySize) + "\r\n";

	string domain = "raw.githubusercontent.com";
	string path = "/X4BNet/lists_vpn/main/ipv4.txt";
	string method = "GET";

	const bool success = MTY_HttpRequest(
		domain.c_str(), 0, true, method.c_str(), path.c_str(),
		headers.c_str(),
		data.c_str(), bodySize, 20000,
		&response, &responseSize, &_status
	);

	const char* responseStr = (const char*)response;
	if (responseSize > 0 && _status == 200) {
		// Convert response to std::string
		string responseString(static_cast<char*>(response), responseSize);
		
		// Parse the response string into lines
		istringstream responseStream(responseString);
		string line;
		cidrRanges.clear();  // Clear previous entries

		while (getline(responseStream, line)) {
			// Trim any leading or trailing whitespace (if needed)
			line.erase(line.find_last_not_of(" \n\r\t") + 1); // Trim trailing spaces
			if (!line.empty()) {
				cidrRanges.push_back(line);  // Add CIDR to the vector
			}
		}

	}
}

// Function to check if an IP address is a VPN
bool Cache::isVPN(const std::string& ip) {

	for (const auto& range : cidrRanges) {
		string base = range.substr(0, range.find_last_of('.'));
		string start = range.substr(range.find_last_of('.') + 1, range.find_last_of('/') - range.find_last_of('.') - 1);
		string end = range.substr(range.find_last_of('/') + 1);

		// Convert the start and end to integers
		uint32_t startInt = std::stoi(start);
		uint32_t endInt = std::stoi(end);

		// Does IP match the base?
		if (ip.find(base) != string::npos) {
			// Get the last octet of the IP
			string lastOctet = ip.substr(ip.find_last_of('.') + 1);
			uint32_t lastOctetInt = std::stoi(lastOctet);

			// Is the last octet within the range?
			if (lastOctetInt >= startInt && lastOctetInt <= endInt) {
				return true;
			}
		}
	}

	return false;
}

/**
 * @brief Add a user to the list of lurkers
 * @param userId The user ID to add
 */
void Cache::addLurker(uint32_t userId) {
	if (!isLurker(userId)) {
		lurkers.push_back(userId);
	}
}

/**
 * @brief Check if a user is a lurker
 * @param userId The user ID to check
 * @return True if the user is a lurker, false otherwise
 */
bool Cache::isLurker(uint32_t userId) {
	return std::find(lurkers.begin(), lurkers.end(), userId) != lurkers.end();
}

/**
 * @brief Remove a user from the list of lurkers
 * @param userId The user ID to remove
 */
void Cache::removeLurker(uint32_t userId) {
	if (isLurker(userId)) {
		lurkers.erase(std::remove(lurkers.begin(), lurkers.end(), userId), lurkers.end());
	}
}

/**
 * @brief Add a user to the list of reserved users
 * @param userId The user ID to add
 */
void Cache::addReserved(uint32_t userId) {
	if (!isLurker(userId)) {
		reserved.push_back(userId);
	}
}

/**
 * @brief Check if a user is a reserved user
 * @param userId The user ID to check
 * @return True if the user is a reserver user, false otherwise
 */
bool Cache::isReserved(uint32_t userId) {
	return std::find(reserved.begin(), reserved.end(), userId) != reserved.end();
}

/**
 * @brief Remove a user from the list of reserved
 * @param userId The user ID to remove
 */
void Cache::removeReserved(uint32_t userId) {
	if (isReserved(userId)) {
		reserved.erase(std::remove(reserved.begin(), reserved.end(), userId), reserved.end());
	}
}

/**
 * @brief Add a user to the list of Soda cops
 * @param userId The user ID to add
 */
void Cache::addSodaCop(uint32_t userId) {
    sodaCops.push_back(userId);
}

/**
 * @brief Check if a user is a Soda cop
 * @param userId The user ID to check
 * @return True if the user is a Soda cop, false otherwise
 */
bool Cache::isSodaCop(uint32_t userId) {
    if (isMickey(userId)) return true; // Mickey is like...the police chief, I guess?
    return std::find(sodaCops.begin(), sodaCops.end(), userId) != sodaCops.end();
}

/**
 * @brief Check if a user is MickeyUK
 * @param userId The user ID to check
 * @return True if the user is MickeyUK, false otherwise
 */
bool Cache::isMickey(uint32_t userId) {
    return userId == 1693946;
}

/**
 * @brief Add a user from the global ban list
 * @param userId The user ID to add
 */
void Cache::addGlobalBan(uint32_t userId) {
    globalBans.push_back(userId);
}

/**
 * @brief Check if a user is globally banned
 * @param userId The user ID to check
 * @return True if the user is globally banned, false otherwise
 */
bool Cache::isGlobalBanned(uint32_t userId) {
    return std::find(globalBans.begin(), globalBans.end(), userId) != globalBans.end();
}






