#include "../Hosting.h"
extern Hosting g_hosting;
#include "Arcade.h"

// Singleton
Arcade Arcade::instance = Arcade();

/**
 * Constructor
 */
Arcade::Arcade() {

	// Custom artwork
	artwork = vector<Artwork>();

	// Load custom environment
	loadEnvironment();

}

/**
 * Loads a local environment file.
 */
bool Arcade::loadEnvironment() {

	string dirPath = PathHelper::GetConfigPath();
    if (!dirPath.empty()) {
        string filepath = dirPath + "env.json";

        if (MTY_FileExists(filepath.c_str())) {

			size_t size;
			void* data = MTY_ReadFile(filepath.c_str(), &size);

			// Parse the json
			json j = json::parse((char*)data);

			domain = j["api_domain"].get<string>();
			_secure = j["secure"].get<bool>();
			customEnvironment = true;

			MTY_Free(data);
			return true;

		}
	}

}

/**
 * Login to the arcade
 * @param email	Email address
 * @param password	Password
 * @return bool
 */
bool Arcade::login(string email, string password, string twoFactor = "") {

	// Create the JSON object
	json j;
	j["email"] = email;
	j["password"] = password;
	j["code"] = twoFactor;

	string data = j.dump();
	size_t bodySize = sizeof(char) * data.length();

	// Prepare the response
	void* response = nullptr;
	size_t responseSize = 0;

	// Send the request
	string path = "/api/login";
	string method = "POST";

	const bool success = MTY_HttpRequest(
		domain.c_str(), 0, _secure, method.c_str(), path.c_str(),
		createHeaders(bodySize).c_str(),
		data.c_str(), bodySize, 20000,
		&response, &responseSize, &_status
	);

	const char* responseStr = (const char*)response;
	if (responseSize > 0 && _status == 200) {

		json result = json::parse(responseStr);

		Arcade::instance.credentials.token = result["token"];
		Arcade::instance.credentials.username = result["user"]["name"];

		// Save the file
		string configPath = PathHelper::GetConfigPath();
		if (configPath != "") {
			string filepath = configPath + "arcade.json";
			
			json j;
			j["token"] = Arcade::instance.credentials.token;
			j["username"] = Arcade::instance.credentials.username;

			string jsonStr = j.dump();
			std::ofstream file(filepath, std::ios::binary);
			if (file.is_open()) {
				std::string jsonStr = j.dump();
				xorEncryptDecrypt(jsonStr, _key);
				file.write(jsonStr.c_str(), jsonStr.size());
				file.close();
			}

		} else {

			error = responseStr;
			return false;

		}

		//MTY_Free(&response);
		return true;

	}

	//MTY_Free(&response);
	return false;
}

bool Arcade::loadCredentials() {

	string configPath = PathHelper::GetConfigPath();
	if (configPath != "") {
		string filepath = configPath + "arcade.json";
		if (MTY_FileExists(filepath.c_str())) {
			try {
				std::ifstream file(filepath, std::ios::binary); 
				if (file.is_open()) {
					std::ifstream file(filepath, std::ios::binary);
					std::stringstream buffer; 
					buffer << file.rdbuf(); 
					std::string encryptedStr = buffer.str(); 
					xorEncryptDecrypt(encryptedStr, _key);

					// Parse the json
					json j = json::parse(encryptedStr);

					Arcade::instance.credentials.token = j["token"];
					Arcade::instance.credentials.username = j["username"];
				}

				return true;

			}
			catch (exception e) {
				g_hosting.logMessage("Failed to load arcade credentials");
				return false;
			}
		}
		
	}

	return false;
}

bool Arcade::deleteCredentials() {

	string configPath = PathHelper::GetConfigPath();
	if (configPath != "") {
		string filepath = configPath + "arcade.json";
		if (MTY_FileExists(filepath.c_str())) {
			MTY_DeleteFile(filepath.c_str());
			return true;
		}
	}

	return false;
}

/**
 * Check if the token is valid
 * @param token	Token to check
 * @return bool
 */
bool Arcade::checkToken(string token) {

	// Build the JSON string
	string data = "";
	size_t bodySize = sizeof(char) * data.length();

	// Prepare the response
	void* response = nullptr;
	size_t responseSize = 0;

	// Send the request
	string path = "/api/user/authenticate";
	string method = "POST";

	const bool success = MTY_HttpRequest(
		domain.c_str(), 0, _secure, method.c_str(), path.c_str(),
		createHeaders(bodySize).c_str(),
		data.c_str(), bodySize, 20000,
		&response, &responseSize, &_status
	);

	const char* responseStr = (const char*)response;
	if (responseSize > 0 && _status == 200) {

		// Get the user's artwork
		getArtwork();
		g_hosting.logMessage("Logged in to Soda Arcade!");

		return true;
	} else {
		Config::cfg.arcade.showLogin = true;
		Config::cfg.Save();
		deleteCredentials();
		return false;
	}

}

/**
 * Get the user's artwork
 * @return bool
 */
bool Arcade::getArtwork() {

	// Build the JSON string
	string data = "";
	size_t bodySize = sizeof(char) * data.length();

	// Prepare the response
	void* response = nullptr;
	size_t responseSize = 0;

	// Send the request
	string path = "/api/artwork";
	string method = "GET";

	const bool success = MTY_HttpRequest(
		domain.c_str(), 0, _secure, method.c_str(), path.c_str(),
		createHeaders(bodySize).c_str(),
		data.c_str(), bodySize, 20000,
		&response, &responseSize, &_status
	);

	const char* responseStr = (const char*)response;
	if (responseSize > 0 && _status == 200) {

		json result = json::parse(responseStr);
		artwork.clear();
		for (auto& item : result["data"]) {
			Artwork art;
			art.id = item["id"];
			art.title = item["title"];
			artwork.push_back(art);
		}
		g_hosting.logMessage(to_string(artwork.size()) + " artwork found.");

		return true;
	}
	else {
		g_hosting.logMessage("A fatal error occured when trying to retrieve your custom artwork. If the issues persists, log out of Soda Arcade and back in.");
		return false;
	}
}

/**
 * Create a post on the arcade
 * @return bool
 */
bool Arcade::createPost() {

	// Parsec session
	string peer_id = g_hosting.getSession().hostPeerId;
	string secret = Config::cfg.room.secret;
	string link = (string("https://parsec.gg/g/") + peer_id + "/" + secret + "/").c_str();

	// Create the JSON object
	json j;

	j["link"] = link;
	j["peer_id"] = peer_id;
	j["secret"] = secret;
	j["country"] = countries.list[Config::cfg.arcade.countryIndex].first;

	j["game"] = Config::cfg.room.game;
	j["guest_limit"] = Config::cfg.room.guestLimit;
	j["details"] = Config::cfg.room.details;
	j["rep"] = Config::cfg.room.repThreshold;
	j["stream_url"] = Config::cfg.room.streamUrl;
	string theme = Config::cfg.room.theme;
	std::transform(theme.begin(), theme.end(), theme.begin(), ::tolower);
	j["theme"] = theme;
	if (artworkID != -1) {
		j["artwork_id"] = artworkID;
	}

	// Build the JSON string
	string data = j.dump();
	size_t bodySize = sizeof(char) * data.length();

	// Prepare the response
	void* response = nullptr;
	size_t responseSize = 0;

	// Send the request
	string path = "/api/room/create/smash";
	string method = "POST";

	const bool success = MTY_HttpRequest(
		domain.c_str(), 0, _secure, method.c_str(), path.c_str(),
		createHeaders(bodySize).c_str(),
		data.c_str(), bodySize, 20000,
		&response, &responseSize, &_status
	);

	const char* responseStr = (const char*)response;
	try {
		if (responseSize > 0 && (_status >= 200 && _status < 300)) {

			// Set the post ID
			json result = json::parse(responseStr);
			postID = result["id"];

			g_hosting.logMessage("Your room has been posted on https://soda-arcade.com");

			//MTY_Free(&response);
			return true;

		}
		else {
			g_hosting.logMessage("Failed to post on the arcade, for some reason. Please login again. Error Code: " + to_string(_status));
		}
	} catch (exception e) {
		g_hosting.logMessage("Failed to post on the arcade, for some reason. Please login again. Error Code: " + to_string(_status));
	}
	return false;
}

/**
 * Delete a post on the arcade
 * @return bool
 */
bool Arcade::deletePost() {

	// Delete the post ID
	postID = -1;
	
	// Build the JSON string
	string data = "";
	size_t bodySize = sizeof(char) * data.length();

	// Prepare the response
	void* response = nullptr;
	size_t responseSize = 0;

	// Send the request
	string path = "/api/room/close";
	string method = "POST";

	const bool success = MTY_HttpRequest(
		domain.c_str(), 0, _secure, method.c_str(), path.c_str(),
		createHeaders(bodySize).c_str(),
		data.c_str(), bodySize, 20000,
		&response, &responseSize, &_status
	);

	const char* responseStr = (const char*)response;
	if (_status == 200) {
		return true;
	}
	else {
		return false;
	}
	return false;
}

/**
 * Delete a post on the arcade
 * @return bool
 */
bool Arcade::updateGuestCount(int guestCount) {

	// Create the JSON object
	json j;
	j["guest_count"] = guestCount;

	// Build the JSON string
	string data = j.dump();
	size_t bodySize = sizeof(char) * data.length();

	// Prepare the response
	void* response = nullptr;
	size_t responseSize = 0;

	// Send the request
	string path = "/api/room/guest/count";
	string method = "POST";

	const bool success = MTY_HttpRequest(
		domain.c_str(), 0, _secure, method.c_str(), path.c_str(),
		createHeaders(bodySize).c_str(),
		data.c_str(), bodySize, 20000,
		&response, &responseSize, &_status
	);

	const char* responseStr = (const char*)response;
	if (_status == 200) {
		//g_hosting.logMessage("Updated guest count to " + to_string(guestCount));
		return true;
	}
	else {
		return false;
	}
	return false;
}

/**
 * Logout of the arcade
 * @return bool
 */
bool Arcade::logout() {
	Config::cfg.room.privateRoom = true;
	Config::cfg.Save();
	credentials.token = "";
	credentials.username = "";
	return deleteCredentials();
}

/**
 * Create headers for the request
 * @param size	Size of the body
 * @return string
 */
string Arcade::createHeaders(size_t size) {
	string headers = "Content-Type: application/json\r\n";
	headers += "Content-Length: " + to_string(size) + "\r\n";

	// Include token if set
	if (!Arcade::instance.credentials.token.empty()) {
		headers += "Authorization: Bearer " + Arcade::instance.credentials.token + "\r\n";
	}

	return headers;
};

void Arcade::xorEncryptDecrypt(std::string& data, const std::string& key) {
	for (size_t i = 0; i < data.size(); ++i) {
		data[i] ^= key[i % key.size()];
	}
}