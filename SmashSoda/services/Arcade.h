#pragma once
#include <nlohmann/json.hpp>
#include "matoya.h"
#include "Countries.h"
#include "PathHelper.h"

using namespace std;

/// <summary>
/// Handles Soda Arcade stuff
/// </summary>
class Arcade {
private:
	bool _secure = true;	/// Whether or not to use HTTPS
	uint16_t _status = 0;	/// The HTTP status code of the last request
	string _key = "AllWorkAndNoPlayMakesMickeyADullBoy**";	/// The encryption key for credentials

	string createHeaders(size_t size);
	void xorEncryptDecrypt(std::string& data, const std::string& key);
public:
	class Artwork {
	public:
		int id = 0;
		string title = "";
	};

	class Credentials {
	public:
		string token = "";
		string username = "";
	};

	string domain = "soda-arcade.com"; /// The domain of the server
	bool customEnvironment = false;	/// Whether we are using custom .env
	string error = "";
	Credentials credentials = Credentials();
	static Arcade instance;
	vector<Artwork> artwork = vector<Artwork>();
	Countries countries = Countries();
	int postID = -1;		// The ID of the post associated with the room
	int artworkID = -1;		// The ID of the artwork associated with the room

	Arcade();
	
	bool loadEnvironment();
	bool loadCredentials();
	bool deleteCredentials();
	bool logout();
	bool login(string email, string password, string twoFactor);
	bool checkToken(string token);

	bool createPost();
	bool deletePost();

	bool updateGuestCount(int count);

	bool getArtwork();
};


