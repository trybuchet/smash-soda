#pragma once

#include <string>
#include <sstream>
#include <chrono>
#include <vector>
#include <algorithm>
#include <iostream>
#include <unordered_map>
#include <Windows.h>
#include <mmeapi.h>
#include <codecvt>
#include "Stringer.h"
#include "matoya.h"

using namespace chrono;
using sec = chrono::duration<double>;

class SFXList
{
public:
	enum class SFXPlayResult
	{
		OK = 0,
		COOLDOWN,
		NOT_FOUND
	};

	class SFX
	{
	public:
		string path = "";
		string tag = "";
		uint32_t cooldown = 0;
	};

	void init(const char* jsonPath);
	int64_t getRemainingCooldown(const string& tag = "");
	SFXPlayResult play(const string tag);
	const string loadedTags();
	const size_t size();

private:
	steady_clock::time_point _lastUseTimestamp;
	SFX _lastSFX;
	unordered_map<string, steady_clock::time_point> _lastUseByTag;
	vector<SFX> _sfxList;
	string _loadedTags;
	int64_t _lastCooldown;
};


