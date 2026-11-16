#pragma once
#include <atomic>
#include <nlohmann/json.hpp>
#include "matoya.h"
#include "Countries.h"
#include "PathHelper.h"

using namespace std;
using json = nlohmann::json;

/// <summary>
/// Handles Soda Arcade stuff
/// </summary>
class Arcade {
private:
	uint16_t _status = 0;	/// The HTTP status code of the last request

	string buildApiPath(const string& endpoint) const;
	string createHeaders(size_t size, const string& contentType = "application/json");
	void xorEncryptDecrypt(std::string& data, const std::string& key);

	/// Sends a JSON request; fills `status` and `body` (parsed, unwrapped from `data` when present).
	bool apiRequest(const string& method, const string& path, const string& requestBody, uint16_t& status, json& body);
	/// Reads the server's onboarding state object (from /auth/me or a 428 response).
	void applyOnboardingState(const json& onboarding);
	/// Reads hosting-rules fields from a user payload.
	void applyHostingTermsState(const json& user);
public:
	/// What the Soda Arcade account still needs before rooms can be listed.
	class AccountState {
	public:
		bool known = false;                 // Loaded from the server at least once
		bool onboardingRequired = false;    // Date of birth and/or Terms acceptance outstanding
		bool needsDateOfBirth = false;
		bool needsTerms = false;
		string termsVersion = "";
		bool hostingTermsAccepted = false;
		string hostingTermsVersion = "";
		string ageStatus = "unknown";       // 'adult' | 'minor' | 'unknown' (only adults can host mature rooms)
	};

	enum class OnboardingResult { Completed, AccountClosed, Failed };
	class Artwork {
	public:
		int id = 0;
		string title = "";
		string url = "";
	};

	class Credentials {
	public:
		string token = "";
		string username = "";
	};

	string error = "";
	Credentials credentials = Credentials();
	static Arcade instance;
	vector<Artwork> artwork = vector<Artwork>();
	Countries countries = Countries();
	int postID = -1;		// The ID of the post associated with the room
	int artworkID = -1;		// The ID of the artwork associated with the room

	AccountState account = AccountState();
	std::atomic<bool> showOnboarding{ false };    // UI: show the account setup window
	std::atomic<bool> showHostingRules{ false };  // UI: show the hosting rules window

	bool fetchAccountState();
	/// Logged in, account state loaded, and no onboarding outstanding: safe to use account features.
	bool isReady() const;
	OnboardingResult completeOnboarding(const string& dateOfBirth);
	bool acceptHostingTerms();
	/// Website URL for a path, e.g. "/terms" (built from the configured API host).
	string webUrl(const string& path) const;

	Arcade();
	
	bool loadCredentials();
	bool deleteCredentials();
	bool logout();
	bool login(string email, string password, string twoFactor);
	bool checkToken(string token);

	bool createPost();
	bool deletePost();

	bool updateGuestCount(int count);
	bool uploadSnapshot();

	bool getArtwork();
};


