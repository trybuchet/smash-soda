#include "SFXList.h"
#include <cmath>

namespace {
	string NormalizeSfxKey(const string& value) {
		string key = Stringer::toLower(value);
		const size_t slashPos = key.find_last_of("/\\");
		if (slashPos != string::npos) {
			key = key.substr(slashPos + 1);
		}

		const size_t dotPos = key.find_last_of('.');
		if (dotPos != string::npos && dotPos > 0) {
			key = key.substr(0, dotPos);
		}

		return key;
	}

	bool MatchesSfx(const SFXList::SFX& sfx, const string& query) {
		if (Stringer::compareNoCase(sfx.tag, query) == 0 || Stringer::compareNoCase(sfx.path, query) == 0) {
			return true;
		}

		const string normalizedQuery = NormalizeSfxKey(query);
		return NormalizeSfxKey(sfx.tag) == normalizedQuery || NormalizeSfxKey(sfx.path) == normalizedQuery;
	}
}

void SFXList::init(const char* jsonPath)
{
	_lastUseTimestamp = steady_clock::now();
	_lastCooldown = 0;
	_lastSFX = SFX();
	stringstream tags;

	// Clear the list
	_sfxList.clear();
	_lastUseByTag.clear();

	// Clear the loaded tags
	_loadedTags.clear();

	bool fileExists = MTY_FileExists(jsonPath);

	if (fileExists) {
		try
		{
			MTY_JSON* json = MTY_JSONReadFile(jsonPath);
			const MTY_JSON* sfxArray = MTY_JSONObjGetItem(json, "sfx");

			uint32_t size = MTY_JSONGetLength(sfxArray);

			for (size_t i = 0; i < size; ++i)
			{
				SFX sfx;
				const MTY_JSON* ji = MTY_JSONArrayGetItem(sfxArray, i);

				char path[1024];
				bool pathSuccess = MTY_JSONObjGetString(ji, "path", path, 1024);
				if (pathSuccess && path != nullptr) sfx.path = path;

				char tag[1024];
				bool tagSuccess = MTY_JSONObjGetString(ji, "tag", tag, 1024);
				if (tagSuccess && tag != nullptr) sfx.tag = tag;

				uint32_t cooldown = 0;
				bool cooldownSuccess = MTY_JSONObjGetUInt(ji, "cooldown", &cooldown);
				if (cooldownSuccess) sfx.cooldown = cooldown;

				if (pathSuccess && tagSuccess && cooldownSuccess)
				{
					_sfxList.push_back(sfx);
				}
			}

			sort(_sfxList.begin(), _sfxList.end(), [](const SFX a, const SFX b) {
				return Stringer::compareNoCase(a.tag, b.tag) < 0;
			});

			for (size_t i = 0; i < _sfxList.size(); ++i)
			{
				if (i > 0)
				{
					tags << ", ";
				}
				tags << NormalizeSfxKey(_sfxList[i].tag);
			}

			_loadedTags = tags.str();
			tags.clear();
			MTY_JSONDestroy(&json);
		}
		catch (const std::exception&)
		{}
	}
}

int64_t SFXList::getRemainingCooldown(const string& tag)
{
	const string targetTag = tag.empty() ? _lastSFX.tag : tag;
	if (targetTag.empty()) {
		return 0;
	}

	for (const SFX& sfx : _sfxList) {
		if (!MatchesSfx(sfx, targetTag)) {
			continue;
		}

		auto it = _lastUseByTag.find(sfx.tag);
		if (it == _lastUseByTag.end()) {
			return 0;
		}

		const sec delta = steady_clock::now() - it->second;
		const double remaining = static_cast<double>(sfx.cooldown) - delta.count();
		return remaining > 0.0 ? static_cast<int64_t>(ceil(remaining)) : 0;
	}

	return 0;
}

SFXList::SFXPlayResult SFXList::play(const string tag)
{
	vector<SFX>::iterator it = _sfxList.begin();
	for (; it != _sfxList.end(); ++it)
	{
		if (MatchesSfx(*it, tag))
		{
			if (getRemainingCooldown((*it).tag) > 0)
			{
				return SFXPlayResult::COOLDOWN;
			}

			string path = string("./SFX/custom/") + string((*it).path);
			wstring wide(path.begin(), path.end());
			PlaySound(wide.c_str(), NULL, SND_FILENAME | SND_NODEFAULT | SND_ASYNC | SND_SYSTEM);
			_lastCooldown = (*it).cooldown;
			_lastUseTimestamp = steady_clock::now();
			_lastSFX = (*it);
			_lastUseByTag[(*it).tag] = _lastUseTimestamp;
			return SFXPlayResult::OK;
		}
	}

	return SFXPlayResult::NOT_FOUND;
}

const string SFXList::loadedTags()
{
	return _loadedTags;
}

const size_t SFXList::size()
{
	return _sfxList.size();
}
