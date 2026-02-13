#pragma once

#include "../GameData.h"
#include "Stringer.h"
#include "PathHelper.h"
#include "matoya.h"
#include <functional>
#include <vector>

using namespace std;

class GameDataList
{
public:
	GameDataList();
	GameDataList(const std::vector<GameData> games);
	bool add(GameData game);
	bool find(uint32_t gameID, function<void(GameData& game)> callback = nullptr);
	bool find(string name, function<void(GameData& game)> callback = nullptr);
	bool pop(uint32_t gameID, function<void(GameData& game)> callback = nullptr);
	bool pop(string name, function<void(GameData& game)> callback = nullptr);
	const bool remove(const uint32_t gameID, function<void(GameData&)> callback);
	const bool remove(string name, function<void(GameData&)> callback);
	vector<GameData>& getGames();
	static vector<GameData> LoadFromFile();
	bool SaveToFile();

protected:
	bool findIterator(uint32_t gameID, function<void(vector<GameData>::iterator game)> callback = nullptr);
	bool findIterator(string name, function<void(vector<GameData>::iterator game)> callback = nullptr);
	vector<GameData> _games;
};




