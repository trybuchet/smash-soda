#include "BanList.h"
#include "Stringer.h"
#include "../Core/Cache.h"

BanList::BanList()
{
}

BanList::BanList(std::vector<GuestData> bannedUsers)
	: GuestDataList(bannedUsers)
{
}

bool BanList::ban(GuestData user)
{
	bool added = GuestDataList::add(user);
	if (added) {
        SaveToFile();

        // Ban the user's IP address
		/*std::string ip = Cache::cache.getUserIpAddress(user.userID);
		if (ip != "") {
			Cache::cache.banIPAddress(ip);
		}*/

	}
	return added;
}

const bool BanList::unban(const uint32_t userID) {

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

const bool BanList::unban(string guestName) {

    vector<GuestData>::iterator gi = _guests.begin();
	for (; gi != _guests.end(); ++gi) {
		if ((*gi).name == guestName) {
			_guests.erase(gi);
			SaveToFile();
			return true;
		}
	}

	return false;

}

const bool BanList::isBanned(const uint32_t userID)
{
	return find(userID);
}

const bool BanList::isBanned(const string guestName)
{
	return find(guestName);
}

vector<GuestData>& BanList::getGuests()
{
	return GuestDataList::getGuests();
}

/**
 * @brief Load the banned users list from file
 */
vector<GuestData> BanList::LoadFromFile() {
	vector<GuestData> result;

    string dirPath = PathHelper::GetConfigPath();
    if (!dirPath.empty()) {
        string filepath = dirPath + "banned.json";

        if (MTY_FileExists(filepath.c_str())) {
            MTY_JSON* json = MTY_JSONReadFile(filepath.c_str());
            uint32_t size = MTY_JSONGetLength(json);

            for (size_t i = 0; i < size; i++) {
                const MTY_JSON* guest = MTY_JSONArrayGetItem(json, (uint32_t)i);

                char name[128] = "";
                uint32_t userID = 0;
                char reason[128] = "";
                bool nameSuccess = MTY_JSONObjGetString(guest, "name", name, 128);
                bool userIDSuccess = MTY_JSONObjGetUInt(guest, "userID", &userID);
                bool reasonSuccess = MTY_JSONObjGetString(guest, "reason", reason, 128);

                if (nameSuccess && userIDSuccess) {
                    if (reasonSuccess) {
                        result.push_back(GuestData(name, userID, reason));
                    } else {
                        result.push_back(GuestData(name, userID, ""));
                    }
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
 * @brief Save the banned users list to file
 */
bool BanList::SaveToFile() {
	string dirPath = PathHelper::GetConfigPath();
    if (!dirPath.empty()) {
        string filepath = dirPath + "banned.json";

        MTY_JSON* json = MTY_JSONArrayCreate();

        vector<GuestData>::iterator gi = _guests.begin();
        for (; gi != _guests.end(); ++gi) {
            MTY_JSON* guest = MTY_JSONObjCreate();

            MTY_JSONObjSetString(guest, "name", (*gi).name.c_str());
            MTY_JSONObjSetUInt(guest, "userID", (*gi).userID);
            MTY_JSONObjSetString(guest, "reason", (*gi).reason.c_str());
            MTY_JSONArrayAppendItem(json, guest);
        }

        MTY_JSONWriteFile(filepath.c_str(), json);
        MTY_JSONDestroy(&json);

        return true;
    }

    return false;
}



