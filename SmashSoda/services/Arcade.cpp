#include "../Hosting.h"
extern Hosting g_hosting;
#include "Arcade.h"
#include "ScreenshotService.h"
#include "../core/Cache.h"

// Singleton
Arcade Arcade::instance = Arcade();

/**
 * Constructor
 */
Arcade::Arcade() {

	// Custom artwork
	artwork = vector<Artwork>();

}

/** Join an endpoint route to the configured API base path. */
string Arcade::buildApiPath(const string& endpoint) const {
	const string route = !endpoint.empty() && endpoint[0] == '/' ? endpoint : "/" + endpoint;
	return Cache::cache.apiBasePath + route;
}

/** Website URL for a path on the configured Soda Arcade host. */
string Arcade::webUrl(const string& path) const {
	string url = string(Cache::cache.apiSecure ? "https://" : "http://") + Cache::cache.apiDomain;
	if (Cache::cache.apiPort != 0) {
		url += ":" + to_string(Cache::cache.apiPort);
	}
	return url + path;
}

bool Arcade::apiRequest(const string& method, const string& path, const string& requestBody, uint16_t& status, json& body) {
	void* response = nullptr;
	size_t responseSize = 0;
	status = 0;
	body = json::object();

	const bool sent = MTY_HttpRequest(
		Cache::cache.apiDomain.c_str(), Cache::cache.apiPort, Cache::cache.apiSecure, method.c_str(), path.c_str(),
		createHeaders(requestBody.length()).c_str(),
		requestBody.empty() ? nullptr : requestBody.c_str(), requestBody.length(), 20000,
		&response, &responseSize, &status
	);

	if (responseSize > 0 && response != nullptr) {
		try {
			json parsed = json::parse(string(static_cast<const char*>(response), responseSize));
			// API responses are normalized to { data: ... }; keep top-level fields as a fallback.
			body = parsed.contains("data") && parsed["data"].is_object() ? parsed["data"] : parsed;
			if (parsed.is_object()) {
				for (const char* key : { "message", "code", "errors", "onboarding", "account_closed", "hosting_terms_version" }) {
					if (parsed.contains(key) && !body.contains(key)) body[key] = parsed[key];
				}
			}
		}
		catch (const std::exception&) {
			// Leave body empty for non-JSON responses.
		}
	}
	if (response != nullptr) {
		MTY_Free(response);
	}
	return sent;
}

void Arcade::applyOnboardingState(const json& onboarding) {
	if (!onboarding.is_object()) return;
	account.needsDateOfBirth = onboarding.value("needs_date_of_birth", false);
	account.needsTerms = onboarding.value("needs_terms", false);
	account.termsVersion = onboarding.value("terms_version", account.termsVersion);
	account.onboardingRequired = onboarding.value("required", account.needsDateOfBirth || account.needsTerms);
	account.known = true;
	showOnboarding = account.onboardingRequired;
}

void Arcade::applyHostingTermsState(const json& user) {
	if (!user.is_object()) return;
	if (user.contains("hosting_terms_accepted") && user["hosting_terms_accepted"].is_boolean()) {
		account.hostingTermsAccepted = user["hosting_terms_accepted"].get<bool>();
	}
	if (user.contains("hosting_terms_version") && user["hosting_terms_version"].is_string()) {
		account.hostingTermsVersion = user["hosting_terms_version"].get<string>();
	}
	if (user.contains("age_status") && user["age_status"].is_string()) {
		account.ageStatus = user["age_status"].get<string>();
	}
}

/**
 * Loads what the account still needs (onboarding, hosting rules) from /v2/auth/me.
 * @return bool True when the state was loaded.
 */
bool Arcade::fetchAccountState() {
	uint16_t status = 0;
	json body;
	if (!apiRequest("GET", buildApiPath("/v2/auth/me"), "", status, body) || status != 200) {
		return false;
	}
	applyOnboardingState(body.value("onboarding", json::object()));
	applyHostingTermsState(body.value("user", json::object()));
	account.known = true;
	return true;
}

bool Arcade::isReady() const {
	return !credentials.token.empty() && account.known && !account.onboardingRequired;
}

/**
 * Finishes the required first-login step: date of birth (when missing) and the current Terms.
 * @param dateOfBirth YYYY-MM-DD, ignored when the account already has one.
 */
Arcade::OnboardingResult Arcade::completeOnboarding(const string& dateOfBirth) {
	error.clear();

	json j = json::object();
	if (account.needsDateOfBirth) {
		j["date_of_birth"] = dateOfBirth;
	}
	if (account.needsTerms) {
		j["accept_terms"] = true;
		j["terms_version"] = account.termsVersion;
	}

	uint16_t status = 0;
	json body;
	if (!apiRequest("POST", buildApiPath("/v2/auth/onboarding"), j.dump(), status, body)) {
		error = "Couldn't reach Soda Arcade. Please check your connection and try again.";
		return OnboardingResult::Failed;
	}

	if (status == 200) {
		applyOnboardingState(body.value("onboarding", json::object()));
		applyHostingTermsState(body.value("user", json::object()));
		if (!account.onboardingRequired) {
			g_hosting.logMessage("Your Soda Arcade account is all set up.");
			getArtwork();
		}
		return OnboardingResult::Completed;
	}

	// Under the minimum age: the server has closed the account and revoked every session.
	if (status == 403 && body.value("account_closed", false)) {
		logout();
		account = AccountState();
		showOnboarding = false;
		showHostingRules = false;
		Config::cfg.arcade.showLogin = false;
		Config::cfg.Save();
		error = "Sorry, you're not able to use Soda Arcade, so your account has been closed.";
		g_hosting.logMessage("Your Soda Arcade account has been closed.");
		return OnboardingResult::AccountClosed;
	}

	// Map the server's message keys to readable text.
	string key = body.value("message", "");
	if (body.contains("errors") && body["errors"].is_object()) {
		for (const auto& [field, messages] : body["errors"].items()) {
			if (messages.is_array() && !messages.empty() && messages[0].is_string()) {
				key = messages[0].get<string>();
				break;
			}
		}
	}
	if (key == "auth.onboarding_terms_outdated") {
		fetchAccountState();
		error = "The Terms were updated while you were reading them. Please review them and try again.";
	} else if (key == "auth.onboarding_terms_required") {
		error = "Please confirm you agree to the Terms of Service and Privacy Policy.";
	} else if (key.rfind("auth.date_of_birth", 0) == 0) {
		error = key == "auth.date_of_birth_already_set"
			? "Your date of birth is already set. Contact support if it needs correcting."
			: "Please enter a valid date of birth.";
	} else {
		error = "Soda Arcade couldn't save your details (HTTP " + to_string(status) + "). Please try again.";
	}
	return OnboardingResult::Failed;
}

/**
 * Records acceptance of the current hosting rules on the account.
 */
bool Arcade::acceptHostingTerms() {
	error.clear();
	if (account.hostingTermsVersion.empty()) {
		fetchAccountState();
	}

	json j;
	j["accepted"] = true;
	j["version"] = account.hostingTermsVersion;

	uint16_t status = 0;
	json body;
	if (!apiRequest("POST", buildApiPath("/v2/room/hosting-terms"), j.dump(), status, body) || status != 200) {
		// The version may have changed: refresh it so the next attempt sends the current one.
		fetchAccountState();
		error = "Soda Arcade couldn't save your agreement (HTTP " + to_string(status) + "). Please try again.";
		return false;
	}

	account.hostingTermsAccepted = true;
	showHostingRules = false;
	g_hosting.logMessage("Hosting rules accepted.");
	return true;
}

/**
 * Login to the arcade
 * @param email	Email address
 * @param password	Password
 * @return bool
 */
bool Arcade::login(string email, string password, string twoFactor = "") {
	error.clear();

	// Create the JSON object
	json j;
	j["email"] = email;
	j["password"] = password;
	j["code"] = twoFactor;
	j["remember_me"] = true;

	string data = j.dump();
	size_t bodySize = sizeof(char) * data.length();

	// Prepare the response
	void* response = nullptr;
	size_t responseSize = 0;

	// Send the request
	string path = buildApiPath("/login");
	string method = "POST";

	const bool success = MTY_HttpRequest(
		Cache::cache.apiDomain.c_str(), Cache::cache.apiPort, Cache::cache.apiSecure, method.c_str(), path.c_str(),
		createHeaders(bodySize).c_str(),
		data.c_str(), bodySize, 20000,
		&response, &responseSize, &_status
	);

	if (responseSize > 0 && _status == 200) {
		const string responseBody(static_cast<const char*>(response), responseSize);
		try {
			json result = json::parse(responseBody);
			const json& payload = result.contains("data") && result["data"].is_object()
				? result["data"]
				: result;
			const string token = payload.value("token", "");
			string username;

			// API responses are normalized to { data: { token, user } }, while
			// older deployments returned the payload directly.
			if (payload.contains("user") && payload["user"].is_object()) {
				const json& userPayload = payload["user"];
				username = userPayload.value("name", "");
			}

			if (token.empty() || username.empty()) {
				error = "Unexpected login response from Soda Arcade.";
				if (response != nullptr) {
					MTY_Free(response);
				}
				return false;
			}

			Arcade::instance.credentials.token = token;
			Arcade::instance.credentials.username = username;
		}
		catch (const std::exception& exception) {
			error = "Failed to parse Soda Arcade login response: ";
			error += exception.what();
			if (response != nullptr) {
				MTY_Free(response);
			}
			return false;
		}

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
				xorEncryptDecrypt(jsonStr, Cache::cache.arcadeCredentialsEncryptionKey);
				file.write(jsonStr.c_str(), jsonStr.size());
				file.close();
			}

		} else {

			error = "Unable to determine the Soda Arcade configuration path.";
			if (response != nullptr) {
				MTY_Free(response);
			}
			return false;

		}

		if (response != nullptr) {
			MTY_Free(response);
		}

		// First login on the new terms: date of birth / Terms acceptance may still be needed.
		fetchAccountState();
		getArtwork();	// No-op until the account is ready (onboarding finished)
		return true;

	}

	if (responseSize > 0) {
		const string responseBody(static_cast<const char*>(response), responseSize);
		try {
			const json result = json::parse(responseBody);
			if (result.contains("message") && result["message"].is_string()) {
				error = result["message"].get<string>();
			} else if (result.contains("error") && result["error"].is_string()) {
				error = result["error"].get<string>();
			} else if (result.contains("errors") && result["errors"].is_object()) {
				for (const auto& [field, messages] : result["errors"].items()) {
					if (messages.is_array() && !messages.empty() && messages[0].is_string()) {
						error = messages[0].get<string>();
						break;
					}
				}
			}
		}
		catch (const std::exception&) {
			// Fall back to the raw response below when it is not JSON.
		}
		if (error.empty()) {
			error = responseBody;
		}
	}
	if (error.empty()) {
		error = "Soda Arcade login failed (HTTP " + to_string(_status) + ").";
	}

	if (response != nullptr) {
		MTY_Free(response);
	}
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
					xorEncryptDecrypt(encryptedStr, Cache::cache.arcadeCredentialsEncryptionKey);

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
	if (!token.empty()) {
		Arcade::instance.credentials.token = token;
	}

	// Build the JSON string
	string data = "";
	size_t bodySize = sizeof(char) * data.length();
	string method = "POST";

	vector<string> validatePaths = {
		buildApiPath("/user/authenticate"),
	};

	uint16_t lastStatus = 0;
	bool hadUnauthorized = false;
	void* response = nullptr;
	size_t responseSize = 0;

	for (const string& path : validatePaths) {
		const bool success = MTY_HttpRequest(
			Cache::cache.apiDomain.c_str(), Cache::cache.apiPort, Cache::cache.apiSecure, method.c_str(), path.c_str(),
			createHeaders(bodySize).c_str(),
			data.c_str(), bodySize, 20000,
			&response, &responseSize, &_status
		);

		lastStatus = _status;
		if (_status == 401 || _status == 403) {
			hadUnauthorized = true;
		}

		// Valid session, but the account must finish onboarding before it can be used.
		if (_status == 428) {
			try {
				json result = json::parse(string(static_cast<const char*>(response), responseSize));
				applyOnboardingState(result.value("onboarding", json::object()));
			}
			catch (const std::exception&) {
				account.onboardingRequired = true;
				account.known = true;
				showOnboarding = true;
			}
			fetchAccountState();
			Config::cfg.arcade.showLogin = false;
			Config::cfg.Save();
			g_hosting.logMessage("Finish setting up your Soda Arcade account to list your room.");
			if (response != nullptr) {
				MTY_Free(response);
			}
			return true;
		}

		if (responseSize > 0 && _status == 200) {
			try {
				// /me may include a new token, but client intentionally keeps the stored token.
				const string responseBody(static_cast<const char*>(response), responseSize);
				json result = json::parse(responseBody);
				const json& payload = result.contains("data") && result["data"].is_object()
					? result["data"]
					: result;
				if (payload.contains("user") && payload["user"].is_object()
					&& payload["user"].contains("name") && payload["user"]["name"].is_string()) {
					Arcade::instance.credentials.username = payload["user"]["name"].get<string>();
				}
			}
			catch (const std::exception&) {
				// Non-fatal: auth already succeeded by status code.
			}

			Config::cfg.arcade.showLogin = false;
			Config::cfg.Save();

			// Account state first: artwork is only fetched once onboarding is done.
			fetchAccountState();
			getArtwork();
			g_hosting.logMessage("Logged in to Soda Arcade!");

			if (response != nullptr) {
				MTY_Free(response);
			}
			return true;
		}
	}

	if (hadUnauthorized) {
		Config::cfg.arcade.showLogin = true;
		Config::cfg.Save();
		deleteCredentials();
		return false;
	}

	// Route mismatch (404) should not spam logs when credentials are otherwise still valid for other endpoints.
	if (lastStatus != 404) {
		g_hosting.logMessage("Unable to validate Soda Arcade session. Error Code: " + to_string(lastStatus));
	}
	if (response != nullptr) {
		MTY_Free(response);
	}
	return false;

}

/**
 * Get the user's artwork
 * @return bool
 */
bool Arcade::getArtwork() {

	// Not logged in, account state not loaded yet, or date of birth / Terms still outstanding.
	if (!isReady()) {
		return false;
	}

	// Build the JSON string
	string data = "";
	size_t bodySize = sizeof(char) * data.length();

	// Prepare the response
	void* response = nullptr;
	size_t responseSize = 0;

	// Send the request
	string path = buildApiPath("/artwork");
	string method = "GET";

	const bool success = MTY_HttpRequest(
		Cache::cache.apiDomain.c_str(), Cache::cache.apiPort, Cache::cache.apiSecure, method.c_str(), path.c_str(),
		createHeaders(bodySize).c_str(),
		data.c_str(), bodySize, 20000,
		&response, &responseSize, &_status
	);

	const char* responseStr = (const char*)response;
	if (responseSize > 0 && _status == 200) {

		try {
			json result = json::parse(string(static_cast<const char*>(response), responseSize));
			if (!result.contains("data") || !result["data"].is_array()) {
				g_hosting.logMessage("Failed to load artwork: unexpected media response format.");
				if (response != nullptr) {
					MTY_Free(response);
				}
				return false;
			}

			artwork.clear();
			for (const auto& item : result["data"]) {
				Artwork art;
				art.id = item.value("id", 0);
				// New media model uses `name`; keep `title` fallback for compatibility.
				art.title = item.value("name", item.value("title", string("")));
				art.url = item.value("url", string(""));
				artwork.push_back(art);
			}

			g_hosting.logMessage(to_string(artwork.size()) + " artwork found.");
			if (response != nullptr) {
				MTY_Free(response);
			}
			return true;
		}
		catch (const exception&) {
			g_hosting.logMessage("Failed to parse artwork media response from Soda Arcade.");
			if (response != nullptr) {
				MTY_Free(response);
			}
			return false;
		}
	}

	if (_status == 401) {
		g_hosting.logMessage("Failed to retrieve artwork media: unauthorized.");
	}
	else {
		string responseBody;
		if (response != nullptr && responseSize > 0) {
			responseBody.assign(static_cast<const char*>(response), responseSize);
		}
		g_hosting.logMessage(
			"Failed to retrieve custom artwork (HTTP " + to_string(_status) + ")" +
			(responseBody.empty() ? string(".") : string(": ") + responseBody)
		);
	}

	if (response != nullptr) {
		MTY_Free(response);
	}
	return false;
}

/**
 * Create a post on the arcade
 * @return bool
 */
bool Arcade::createPost() {

	// The server refuses to list rooms until these are done; ask in-app instead of failing.
	if (account.known && account.onboardingRequired) {
		showOnboarding = true;
		g_hosting.logMessage("Finish setting up your Soda Arcade account to list your room.");
		return false;
	}
	if (account.known && !account.hostingTermsAccepted) {
		showHostingRules = true;
		g_hosting.logMessage("Please read and accept the Soda Arcade hosting rules to list your room.");
		return false;
	}

	// Parsec session
	string peer_id = g_hosting.getSession().hostPeerId;
	string secret = Config::cfg.room.secret;
	string link = (string("https://parsec.gg/g/") + peer_id + "/" + secret + "/").c_str();

	// Create the JSON object (new room schema)
	json j;
	j["type"] = "parsec";

	string countryCode = "";
	for (const auto& country : countries.list) {
		if (country.first == Config::cfg.arcade.country || country.second == Config::cfg.arcade.country) {
			countryCode = country.first;
			break;
		}
	}

	if (countryCode.empty()) {
		const int countryIndex = Config::cfg.arcade.countryIndex;
		if (countryIndex >= 0 && countryIndex < static_cast<int>(countries.list.size())) {
			countryCode = countries.list[countryIndex].first;
		}
		else {
			countryCode = "US";
		}
	}

	string theme = Config::cfg.room.theme;
	std::transform(theme.begin(), theme.end(), theme.begin(), ::tolower);

	// Legacy v1 endpoint expects the flat Smash Soda request contract.
	j["link"] = link;
	j["peer_id"] = peer_id;
	j["secret"] = secret;
	j["country"] = countryCode;
	j["game"] = Config::cfg.room.game;
	j["guest_limit"] = Config::cfg.room.guestLimit;
	j["details"] = Config::cfg.room.details;
	j["stream_url"] = Config::cfg.room.streamUrl;
	j["rep"] = Config::cfg.room.repThreshold;

	string previewType = Config::cfg.room.previewType;
	if (previewType != "auto" && previewType != "snapshot" && previewType != "custom") {
		previewType = "auto";
	}
	// The server refuses Custom Artwork without an image; use the game cover instead of not listing the room.
	if (previewType == "custom" && artworkID <= 0) {
		previewType = "auto";
		g_hosting.logMessage("No custom artwork is selected, so your room thumbnail uses Game Cover Art.");
	}
	j["theme"] = theme;
	j["preview_type"] = previewType;

	// Mature (18-rated) listing: only confirmed adults; the server enforces this too.
	j["is_mature"] = Config::cfg.room.matureRoom && account.ageStatus == "adult";

	if (previewType == "custom") {
		j["artwork_id"] = artworkID;
	}

	// Build the JSON string
	string data = j.dump();
	size_t bodySize = sizeof(char) * data.length();

	// Prepare the response
	void* response = nullptr;
	size_t responseSize = 0;

	// Send the request
	string path = buildApiPath("/room/create/smash");
	string method = "POST";

	const bool success = MTY_HttpRequest(
		Cache::cache.apiDomain.c_str(), Cache::cache.apiPort, Cache::cache.apiSecure, method.c_str(), path.c_str(),
		createHeaders(bodySize).c_str(),
		data.c_str(), bodySize, 20000,
		&response, &responseSize, &_status
	);

	const char* responseStr = (const char*)response;
	if (responseSize > 0 && (_status >= 200 && _status < 300)) {
		try {
			json result = json::parse(string(static_cast<const char*>(response), responseSize));
			if (result.contains("data") && result["data"].is_object()) {
				result = result["data"];
			}

			// Store the room ID when available (create/update responses both include it).
			postID = result.value("id", -1);
			if (postID <= 0) {
				g_hosting.logMessage("Failed to parse Soda Arcade room response: missing room ID.");
				if (response != nullptr) {
					MTY_Free(response);
				}
				return false;
			}

			g_hosting.logMessage("Your room has been posted on https://soda-arcade.com");
			if (response != nullptr) {
				MTY_Free(response);
			}
			return true;
		}
		catch (const exception&) {
			g_hosting.logMessage("Failed to parse Soda Arcade room response.");
			if (response != nullptr) {
				MTY_Free(response);
			}
			return false;
		}
	}

	try {
		if (responseSize > 0) {
			json result = json::parse(string(static_cast<const char*>(response), responseSize));

			// Account still needs onboarding (date of birth / Terms).
			if (_status == 428) {
				applyOnboardingState(result.value("onboarding", json::object()));
				showOnboarding = true;
				g_hosting.logMessage("Finish setting up your Soda Arcade account to list your room.");
				if (response != nullptr) {
					MTY_Free(response);
				}
				return false;
			}

			// Hosting rules not accepted yet (or a new version was published).
			if (_status == 403 && result.value("code", "") == "hosting_terms_required") {
				account.known = true;
				account.hostingTermsAccepted = false;
				account.hostingTermsVersion = result.value("hosting_terms_version", account.hostingTermsVersion);
				showHostingRules = true;
				g_hosting.logMessage("Please read and accept the Soda Arcade hosting rules to list your room.");
				if (response != nullptr) {
					MTY_Free(response);
				}
				return false;
			}

			if (result.contains("message") && result["message"].is_string()) {
				g_hosting.logMessage("Failed to post on the arcade: " + result["message"].get<string>() + " (" + to_string(_status) + ")");
				if (response != nullptr) {
					MTY_Free(response);
				}
				return false;
			}
			if (result.contains("errors") && result["errors"].is_object()) {
				for (const auto& [field, messages] : result["errors"].items()) {
					if (messages.is_array() && !messages.empty() && messages[0].is_string()) {
						g_hosting.logMessage("Failed to post on the arcade: " + messages[0].get<string>() + " (" + to_string(_status) + ")");
						if (response != nullptr) {
							MTY_Free(response);
						}
						return false;
					}
				}
			}
		}
	}
	catch (const exception&) {
	}

	g_hosting.logMessage("Failed to post on the arcade, for some reason. Please login again. Error Code: " + to_string(_status));
	if (response != nullptr) {
		MTY_Free(response);
	}
	return false;
}

/**
 * Delete a post on the arcade
 * @return bool
 */
bool Arcade::deletePost() {
	const int roomId = postID;
	if (roomId <= 0) {
		return false;
	}
	
	// Build the JSON string
	string data = "";
	size_t bodySize = sizeof(char) * data.length();

	// Prepare the response
	void* response = nullptr;
	size_t responseSize = 0;

	// Send the request
	string path = buildApiPath("/room/close");
	string method = "POST";

	const bool success = MTY_HttpRequest(
		Cache::cache.apiDomain.c_str(), Cache::cache.apiPort, Cache::cache.apiSecure, method.c_str(), path.c_str(),
		createHeaders(bodySize).c_str(),
		data.c_str(), bodySize, 20000,
		&response, &responseSize, &_status
	);

	const char* responseStr = (const char*)response;
	if (_status >= 200 && _status < 300) {
		postID = -1;
		if (response != nullptr) {
			MTY_Free(response);
		}
		return true;
	}
	else {
		if (response != nullptr) {
			MTY_Free(response);
		}
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
	string path = buildApiPath("/room/guest/count");
	string method = "POST";

	const bool success = MTY_HttpRequest(
		Cache::cache.apiDomain.c_str(), Cache::cache.apiPort, Cache::cache.apiSecure, method.c_str(), path.c_str(),
		createHeaders(bodySize).c_str(),
		data.c_str(), bodySize, 20000,
		&response, &responseSize, &_status
	);

	const char* responseStr = (const char*)response;
	if (_status == 200) {
		//g_hosting.logMessage("Updated guest count to " + to_string(guestCount));
		if (response != nullptr) {
			MTY_Free(response);
		}
		return true;
	}
	else {
		if (response != nullptr) {
			MTY_Free(response);
		}
		return false;
	}
	return false;
}

/**
 * Capture and upload a room snapshot image to Soda Arcade.
 * @return bool
 */
bool Arcade::uploadSnapshot() {
	std::vector<uint8_t> jpegData;
	if (!ScreenshotService::getInstance().captureJpeg(jpegData, 300, 150, 85)) {
		g_hosting.logMessage("Snapshot: capture/encode FAILED (no image to upload).");
		return false;
	}

	void* response = nullptr;
	size_t responseSize = 0;

	// Smash Soda talks to the legacy route set
	// ?snapshot=1 tells the server this is an image upload, so it can never be
	// mistaken for a plain heartbeat (which returns 200 without storing anything).
	string path = buildApiPath("/room/snapshots") + "?snapshot=1";
	string method = "POST";
	const string contentType = "image/jpeg";

	g_hosting.logMessage(
		"Snapshot: sending " + to_string(jpegData.size()) + " bytes (" + contentType + ") to " +
		string(Cache::cache.apiSecure ? "https://" : "http://") + Cache::cache.apiDomain +
		":" + to_string(Cache::cache.apiPort) + path
	);

	_status = 0;
	const bool success = MTY_HttpRequest(
		Cache::cache.apiDomain.c_str(), Cache::cache.apiPort, Cache::cache.apiSecure, method.c_str(), path.c_str(),
		createHeaders(jpegData.size(), contentType).c_str(),
		jpegData.data(), jpegData.size(), 20000,
		&response, &responseSize, &_status
	);

	string responseBody;
	if (response != nullptr && responseSize > 0) {
		responseBody.assign(static_cast<const char*>(response), responseSize);
	}
	if (response != nullptr) {
		MTY_Free(response);
	}

	// Always log what came back, success or not (body trimmed to keep the log readable).
	g_hosting.logMessage(
		string("Snapshot: response HTTP ") + to_string(_status) +
		", request " + (success ? "completed" : "FAILED") +
		", " + to_string(responseSize) + " bytes" +
		(responseBody.empty() ? string(", empty body.") : string(": ") + responseBody.substr(0, 500))
	);

	if (!success || _status < 200 || _status >= 300) {
		g_hosting.logMessage("Snapshot: NOT uploaded (HTTP " + to_string(_status) + ").");
		return false;
	}

	// A 2xx alone doesn't prove the image was stored: the server also answers 200 to
	// a plain heartbeat. Only report success when it confirms the stored image.
	bool stored = false;
	string imageUrl;
	try {
		json result = json::parse(responseBody);
		if (result.contains("data") && result["data"].is_object()) {
			result = result["data"];
		}
		if (result.contains("image_url") && result["image_url"].is_string()) {
			imageUrl = result["image_url"].get<string>();
			stored = !imageUrl.empty();
		}
	}
	catch (const std::exception&) {
		stored = false;
	}

	if (!stored) {
		g_hosting.logMessage(
			"Snapshot: NOT stored - server answered HTTP " + to_string(_status) +
			" but did not return an image_url (see response above)."
		);
		return false;
	}

	g_hosting.logMessage("Snapshot: stored OK at " + imageUrl);
	return true;
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
	account = AccountState();
	showOnboarding = false;
	showHostingRules = false;
	return deleteCredentials();
}

/**
 * Create headers for the request
 * @param size	Size of the body
 * @return string
 */
string Arcade::createHeaders(size_t size, const string& contentType) {
	string headers = "Content-Type: " + contentType + "\r\n";
	headers += "Content-Length: " + to_string(size) + "\r\n";
	headers += "User-Agent: SmashSoda\r\n";

	// Include client credentials when configured; builds without them remain valid.
	if (!Cache::cache.clientAppId.empty() && !Cache::cache.clientAppSecret.empty()) {
		headers += "X-Client-Id: " + Cache::cache.clientAppId + "\r\n";
		headers += "X-Client-Secret: " + Cache::cache.clientAppSecret + "\r\n";
	}

	// Include token if set
	if (!Arcade::instance.credentials.token.empty()) {
		headers += "Authorization: Bearer " + Arcade::instance.credentials.token + "\r\n";
	}

	return headers;
};

void Arcade::xorEncryptDecrypt(std::string& data, const std::string& key) {
	if (key.empty()) {
		return;
	}

	for (size_t i = 0; i < data.size(); ++i) {
		data[i] ^= key[i % key.size()];
	}
}