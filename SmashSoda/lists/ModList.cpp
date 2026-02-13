#include "ModList.h"
#include "Stringer.h"
#include "../Core/Cache.h"
#include "../Tier.h"

ModList::ModList() {}

ModList::ModList(std::vector<GuestData> moddedUsers)
	: GuestDataList(moddedUsers)
{
}

bool ModList::mod(GuestData user) {
	bool added = GuestDataList::add(user);
    Cache::cache.tierList.setTier(user.userID, Tier::MOD);
	if (added) {
		SaveToFile();
	}
	return added;
}

const bool ModList::unmod(const uint32_t userID) {
    
    vector<GuestData>::iterator gi = _guests.begin();
    for (; gi != _guests.end(); ++gi) {
        if ((*gi).userID == userID) {
            _guests.erase(gi);
            SaveToFile();
            return true;
        }
    }

    return false;

}

const bool ModList::unmod(string guestName) {

    vector<GuestData>::iterator gi = _guests.begin();
    for (; gi != _guests.end(); ++gi) {
        if ((*gi).name == guestName) {
            Cache::cache.tierList.setTier((*gi).userID, Tier::GUEST);
            _guests.erase(gi);
            SaveToFile();
            return true;
        }
    }

    return false;

}

const bool ModList::isModded(const uint32_t userID) {
	return find(userID);
}

vector<GuestData>& ModList::getGuests()
{
	return GuestDataList::getGuests();
}

/**
 * @brief Load the modded users list from file
 */
vector<GuestData> ModList::LoadFromFile() {
	vector<GuestData> result;

    string dirPath = PathHelper::GetConfigPath();
    if (!dirPath.empty()) {
        string filepath = dirPath + "mods.json";

        if (MTY_FileExists(filepath.c_str())) {
            MTY_JSON* json = MTY_JSONReadFile(filepath.c_str());
            uint32_t size = MTY_JSONGetLength(json);

            for (size_t i = 0; i < size; i++) {
                const MTY_JSON* guest = MTY_JSONArrayGetItem(json, (uint32_t)i);

                char name[128] = "";
                uint32_t userID = 0;
                bool nameSuccess = MTY_JSONObjGetString(guest, "name", name, 128);
                bool userIDSuccess = MTY_JSONObjGetUInt(guest, "userID", &userID);

                if (nameSuccess && userIDSuccess) {
                    result.push_back(GuestData(name, userID));
                }
            }

            std::sort(result.begin(), result.end(), [](const GuestData a, const GuestData b) {
                return a.userID < b.userID;
                });

            MTY_JSONDestroy(&json);
        }
    }

    return result;
}

/**
 * @brief Save the modded users list to file
 */
bool ModList::SaveToFile() {
	string dirPath = PathHelper::GetConfigPath();
    if (!dirPath.empty()) {
        string filepath = dirPath + "mods.json";

        MTY_JSON* json = MTY_JSONArrayCreate();

        vector<GuestData>::iterator gi = _guests.begin();
        for (; gi != _guests.end(); ++gi) {
            MTY_JSON* guest = MTY_JSONObjCreate();

            MTY_JSONObjSetString(guest, "name", (*gi).name.c_str());
            MTY_JSONObjSetUInt(guest, "userID", (*gi).userID);
            MTY_JSONArrayAppendItem(json, guest);
        }

        MTY_JSONWriteFile(filepath.c_str(), json);
        MTY_JSONDestroy(&json);

        return true;
    }

    return false;
}


