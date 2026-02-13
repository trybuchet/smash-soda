#pragma once

#include <string>

using namespace std;

class SessionCache {
	public:
		enum class SessionType
		{
			PERSONAL = 0,
			THIRD = 1,
			ENTERPRISE = 2
		};

		string sessionID = "";
		string peerID = "";
		SessionType type = SessionType::THIRD;
		uint32_t start = 0;
		uint32_t expiry = 0;
		bool isValid = false;

};