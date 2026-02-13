#include "GameDataList.h"

GameDataList::GameDataList()
{
}

GameDataList::GameDataList(const std::vector<GameData> games)
{
	_games = games;
}

bool GameDataList::add(GameData game)
{
	bool found = find(game.itemID);
	if (!found)
	{
		_games.push_back(game);
		return true;
	}

	return false;
}

bool GameDataList::find(uint32_t itemID, function<void(GameData& game)> callback)
{
	bool found = findIterator(itemID, [&](vector<GameData>::iterator gi) {
		if (callback != nullptr)
		{
			callback(*gi);
		}
		});

	return found;
}

bool GameDataList::find(string name, function<void(GameData& game)> callback)
{
	bool found = findIterator(name, [&](vector<GameData>::iterator gi) {
		if (callback != nullptr)
		{
			callback(*gi);
		}
		});

	return found;
}

bool GameDataList::pop(uint32_t itemID, function<void(GameData& game)> callback)
{
	bool found = findIterator(itemID, [&](vector<GameData>::iterator gi) {
		if (callback != nullptr)
		{
			callback(*gi);
		}
		_games.erase(gi);
		});

	return found;
}

bool GameDataList::pop(string name, function<void(GameData& game)> callback)
{
	bool found = findIterator(name, [&](vector<GameData>::iterator gi) {
		if (callback != nullptr)
		{
			callback(*gi);
		}
		_games.erase(gi);
		});

	return found;
}

const bool GameDataList::remove(const uint32_t itemID, function<void(GameData&)> callback)
{
	bool found = GameDataList::pop(itemID, callback);
	if (found) {
		SaveToFile();
	}
	return found;
}

const bool GameDataList::remove(string name, function<void(GameData&)> callback)
{
	bool found = GameDataList::pop(name, callback);
	if (found) {
		SaveToFile();
	}
	return found;
}

vector<GameData>& GameDataList::getGames()
{
	return _games;
}

/**
 * @brief Load the games list from file
 */
vector<GameData> GameDataList::LoadFromFile() {
	vector<GameData> result;

    string dirPath = PathHelper::GetConfigPath();
    if (!dirPath.empty()) {
        string filepath = dirPath + "games.json";

        if (MTY_FileExists(filepath.c_str())) {
            MTY_JSON* json = MTY_JSONReadFile(filepath.c_str());
            uint32_t size = MTY_JSONGetLength(json);

            for (size_t i = 0; i < size; i++)
            {
                const MTY_JSON* game = MTY_JSONArrayGetItem(json, (uint32_t)i);

                uint32_t itemID = 0;
                char name[128] = "";
                char path[256] = "";
                char parameters[256] = "";
                char thumbnailPath[256] = "";
                char gameID[256] = "";
				bool kiosk = false;
				bool hotseat = false;
				uint32_t seats = 0;

				bool itemIDSuccess = MTY_JSONObjGetUInt(game, "itemID", &itemID);
                bool nameSuccess = MTY_JSONObjGetString(game, "name", name, 128);
                bool pathSuccess = MTY_JSONObjGetString(game, "path", path, 256);
                bool paramSuccess = MTY_JSONObjGetString(game, "parameters", parameters, 256);

				if (itemIDSuccess && nameSuccess && pathSuccess && paramSuccess) {
                    result.push_back(GameData(itemID, name, path, parameters));
                }
            }

            /*std::sort(result.begin(), result.end(), [](const GameData a, const GameData b) {
                return a.gameID < b.gameID;
            });*/

            MTY_JSONDestroy(&json);
        }
    }

    return result;
}

/**
 * @brief Save the games list to file
 */
bool GameDataList::SaveToFile() {
	string dirPath = PathHelper::GetConfigPath();
    if (!dirPath.empty()) {
        string filepath = dirPath + "games.json";

        MTY_JSON* json = MTY_JSONArrayCreate();

        vector<GameData>::iterator gi = _games.begin();
        for (; gi != _games.end(); ++gi) {
            MTY_JSON* game = MTY_JSONObjCreate();

			MTY_JSONObjSetUInt(game, "itemID", (*gi).itemID);
            MTY_JSONObjSetString(game, "name", (*gi).name.c_str());
            MTY_JSONObjSetString(game, "path", (*gi).path.c_str());
            MTY_JSONObjSetString(game, "parameters", (*gi).parameters.c_str());

            MTY_JSONArrayAppendItem(json, game);
        }

        MTY_JSONWriteFile(filepath.c_str(), json);
        MTY_JSONDestroy(&json);

        return true;
    }

    return false;
}


// =============================================================
//
//  Private
//
// =============================================================
bool GameDataList::findIterator(uint32_t itemID, function<void(vector<GameData>::iterator game)> callback)
{
	vector<GameData>::iterator it = _games.begin();

	for (; it != _games.end(); ++it)
	{
		if (itemID == (*it).itemID)
		{
			if (callback != nullptr)
			{
				callback(it);
			}
			return true;
		}
	}

	return false;
}

bool GameDataList::findIterator(string name, function<void(vector<GameData>::iterator game)> callback)
{
	vector<GameData>::iterator it = _games.begin();

	for (; it != _games.end(); ++it)
	{
		if (Stringer::isCloseEnough((*it).name.c_str(), name))
		{
			if (callback != nullptr)
			{
				callback(it);
			}
			return true;
		}
	}

	return false;
}



