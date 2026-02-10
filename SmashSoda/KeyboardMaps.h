#pragma once

//-- CodeSomnia Modified Start--
#include "parsec.h"
#include "./KeyboardMapsUtil.h"
#include "matoya.h"
#include <vector>
#include <map>
#include <nlohmann/json.hpp>
#include "helpers/PathHelper.h"

typedef vector<uint16_t> MappingKeyList_t;
typedef vector<KeyMapInfo> KeySet_t;

using namespace std;
using json = nlohmann::json;

class KeyboardProfile {
public:
	string name;
	uint32_t userID;
	vector<uint16_t> keyMap;
};

class KeyboardMap
{

private:
	KeyboardProfile loadAndValidateProfile(const json& j, const string& filename) {
		KeyboardProfile profile;
		profile.name = j.value("name", "Unnamed Profile");
		profile.userID = j.value("userID", 0);

		if (profile.userID == 0 && filename != "0.json") {
			string userIDStr = filename.substr(0, filename.find(".json"));
			try {
				profile.userID = stoul(userIDStr);
			}
			catch (const std::exception& e) {}
		}

		bool mapIsValid = false;
		if (j.contains("keyMap") && j.at("keyMap").is_array() && j.at("keyMap").size() == defaultKeyMap.size()) {
			try {
				j.at("keyMap").get_to(profile.keyMap);
				mapIsValid = true;
			}
			catch (const json::type_error& e) {
				mapIsValid = false;
			}
		}

		if (!mapIsValid) {
			profile.keyMap = defaultKeyMap;
		}

		return profile;
	}

public:

	vector<KeyboardProfile> profiles;

	// Default keymap
	vector<uint16_t> defaultKeyMap = {
		KEY_A,
		KEY_D,
		KEY_W,
		KEY_S,
		KEY_LEFT,
		KEY_RIGHT,
		KEY_UP,
		KEY_DOWN,
		KEY_KP_4,
		KEY_KP_6,
		KEY_KP_8,
		KEY_KP_5,
		KEY_L,
		KEY_O,
		KEY_K,
		KEY_I,
		KEY_Q,
		KEY_P,
		KEY_F,
		KEY_J,
		KEY_BACKSPACE,
		KEY_ENTER,
		KEY_C,
		KEY_M
	};

	// List of button names
	vector<string> buttonNames = {
		"LLeft",
		"LRight",
		"LUp",
		"LDown",
		"RLeft",
		"RRight",
		"RUp",
		"RDown",
		"DLeft",
		"DRight",
		"DUp",
		"DDown",
		"A",
		"B",
		"X",
		"Y",
		"LB",
		"RB",
		"LT",
		"RT",
		"Back",
		"Start",
		"LThumb",
		"RThumb"
	};

	// Button names lowercase
	vector<string> buttonNamesLower = {
		"lleft",
		"lright",
		"lup",
		"ldown",
		"rleft",
		"rright",
		"rup",
		"rdown",
		"dleft",
		"dright",
		"dup",
		"ddown",
		"a",
		"b",
		"x",
		"y",
		"lb",
		"rb",
		"lt",
		"rt",
		"back",
		"start",
		"lthumb",
		"rthumb"
	};

	/// <summary>
	/// Constructor
	/// </summary>
	KeyboardMap() {
		loadProfiles();
	};

	void loadProfiles() {
		profiles.clear();
		string keyboardDir = PathHelper::GetConfigPath() + "keyboard";

		if (!MTY_FileExists(keyboardDir.c_str())) {
			MTY_Mkdir(keyboardDir.c_str());
		}

		vector<string> filenames = PathHelper::GetFilenames(keyboardDir, true);

		for (const auto& filename : filenames) {
			if (filename.size() > 5 && filename.substr(filename.size() - 5) == ".json") {
				string filePath = keyboardDir + "\\" + filename;
				
				try {
					size_t size;
					void* data = MTY_ReadFile(filePath.c_str(), &size);
					if (!data) continue;

					json j = json::parse((char*)data);
					MTY_Free(data);

					profiles.push_back(loadAndValidateProfile(j, filename));

				} catch (const std::exception& e) {
					// JSON parsing failed for this file, ignore it.
				}
			}
		}

		if (profiles.empty()) {
			// Default Profile
			KeyboardProfile defaultProfile;
			defaultProfile.name = "Default";
			defaultProfile.userID = 0;
			defaultProfile.keyMap = defaultKeyMap;
			profiles.push_back(defaultProfile);
			saveProfile(0);
		}

		KeyboardProfile* current = findProfile(_curPresetID);
		if (current == nullptr) {
			KeyboardProfile* defaultProfile = findProfile(0);
			if (defaultProfile != nullptr) {
				_curPresetID = defaultProfile->userID;
				_curPresetName = defaultProfile->name;
			}
			else if (!profiles.empty()) {
				_curPresetID = profiles[0].userID;
				_curPresetName = profiles[0].name;
			}
		}
	};

	bool saveProfile(uint32_t userID) {
		KeyboardProfile* profile = findProfile(userID);
		if (profile == nullptr) return false;

		string dirPath = PathHelper::GetConfigPath() + "keyboard";
		if (!MTY_FileExists(dirPath.c_str())) {
			MTY_Mkdir(dirPath.c_str());
		}

		string filePath = dirPath + "\\" + to_string(userID) + ".json";

		json j;
		j["name"] = profile->name;
		j["userID"] = profile->userID;
		j["keyMap"] = profile->keyMap;

		string profileString = j.dump(4);
		return MTY_WriteTextFile(filePath.c_str(), "%s", profileString.c_str());
	}

	void saveAllProfiles() {
		for (const auto& profile : profiles) {
			saveProfile(profile.userID);
		}
	}

	/// <summary>
	/// Find a profile by name
	/// </summary>
	/// <param name="name"></param>
	/// <returns></returns>
	KeyboardProfile* findProfile(uint32_t userID) {
		for (KeyboardProfile& profile : profiles) {
			if (profile.userID == userID) {
				return &profile;
			}
		}

		return nullptr;
	};

	// Create a new profile
	bool createProfile(string name, uint32_t userID) {
		if (findProfile(userID) == nullptr) {
			KeyboardProfile newProfile;
			newProfile.name = name;
			newProfile.userID = userID;
			
			KeyboardProfile* defaultProfile = findProfile(0);
			if (defaultProfile != nullptr) {
				newProfile.keyMap = defaultProfile->keyMap;
			} else if (!profiles.empty()) {
                newProfile.keyMap = profiles[0].keyMap;
            } else {
                newProfile.keyMap = defaultKeyMap;
            }

			profiles.push_back(newProfile);
            saveProfile(userID);
			return true;
		}

		return false;
	};

	/// <summary>
	/// Delete a profile by userID
	/// </summary>
	/// <param name="userID"></param>
	/// <returns></returns>
	bool deleteProfile(uint32_t userID) {
		for (int i = 0; i < profiles.size(); i++) {
			if (profiles[i].userID == userID) {
				profiles.erase(profiles.begin() + i);
				
				string dirPath = PathHelper::GetConfigPath() + "keyboard";
				string filePath = dirPath + "\\" + to_string(userID) + ".json";
				if (MTY_FileExists(filePath.c_str())) {
					MTY_DeleteFile(filePath.c_str());
				}
				
				return true;
			}
		}
		return false;
	};

	// Reset a profile to default
	bool resetProfile(uint32_t userID) {
		KeyboardProfile* profile = findProfile(userID);

		if (profile != nullptr) {
			profile->keyMap = defaultKeyMap;
            saveProfile(userID);
			return true;
		}

		return false;
	};

	/// <summary>
	/// Map a button to a key
	/// </summary>
	/// <param name="profileName"></param>
	/// <param name="buttonName"></param>
	/// <param name="key"></param>
	/// <returns></returns>
	bool mapButton(string userName, uint32_t userID, string buttonName, uint16_t key) {
		KeyboardProfile* profile = findProfile(userID);

		if (profile == nullptr) {
			createProfile(userName, userID);
			profile = findProfile(userID);
		}

		if (isValidButtonName(buttonName)) {

			for (int i = 0; i < profile->keyMap.size(); i++) {
				if (profile->keyMap[i] == key) {
					profile->keyMap[i] = 0; // Use 0 for unassigned
				}
			}

			int index = find(buttonNamesLower.begin(), buttonNamesLower.end(), buttonName) - buttonNamesLower.begin();
			profile->keyMap[index] = key;
            saveProfile(userID);
			return true;
		}

		return false;
	};

	bool setButton(string buttonName, uint16_t key) {
		KeyboardProfile* profile = findProfile(_curPresetID);

		if (profile != nullptr) {
			int index = find(buttonNames.begin(), buttonNames.end(), buttonName) - buttonNames.begin();
			profile->keyMap[index] = key;
			return true;
		}

		return false;
	};

	/// <summary>
	/// Check if button name is valid
	/// </summary>
	/// <param name="name"></param>
	/// <returns></returns>
	bool isValidButtonName(string name) {
		return find(buttonNamesLower.begin(), buttonNamesLower.end(), name) != buttonNamesLower.end();
	};

	bool isKeySetForButton(string buttonName, uint16_t key) {
		KeyboardProfile* profile = findProfile(_curPresetID);

		if (profile != nullptr) {
			int index = find(buttonNames.begin(), buttonNames.end(), buttonName) - buttonNames.begin();
			return profile->keyMap[index] == key;
		}

		return false;
	};

	uint16_t getKeyForButton(string buttonName) {
		KeyboardProfile* profile = findProfile(_curPresetID);

		if (profile != nullptr) {
			int index = find(buttonNames.begin(), buttonNames.end(), buttonName) - buttonNames.begin();
			if (index < profile->keyMap.size()) {
                return profile->keyMap[index];
            }
		}

		return 0;
	};

	// Helper function to set a value in the config object
	template <typename T>
	static T setValue(const T& originalValue, const json& newValue) {
		if (!newValue.is_null()) {
			try {
				return newValue.get<T>();
			}
			catch (const json::type_error& e) {
				std::cerr << "Error: Type mismatch when setting config property" << std::endl;
			}
		}
		return originalValue;
	}

	//CodeSomnia : °¢ Å°°ªÀ» °ÔÀÓÆÐµå Å° ¸®½ºÆ®¿¡ Ãß°¡.
	void SetToDefaultKeyInfo(GamePadButtonType_e keyType, uint16_t keyVal)
	{
		switch (keyType)
		{
		case GamePadButtonType_e::LLeft:
			LLeft.push_back(keyVal);
			break;
		case GamePadButtonType_e::LRight:
			LRight.push_back(keyVal);
			break;
		case GamePadButtonType_e::LUp:
			LUp.push_back(keyVal);
			break;
		case GamePadButtonType_e::LDown:
			LDown.push_back(keyVal);
			break;
		case GamePadButtonType_e::RLeft:
			RLeft.push_back(keyVal);
			break;
		case GamePadButtonType_e::RRight:
			RRight.push_back(keyVal);
			break;
		case GamePadButtonType_e::RUp:
			RUp.push_back(keyVal);
			break;
		case GamePadButtonType_e::RDown:
			RDown.push_back(keyVal);
			break;
		case GamePadButtonType_e::DLeft:
			DLeft.push_back(keyVal);
			break;
		case GamePadButtonType_e::DRight:
			DRight.push_back(keyVal);
			break;
		case GamePadButtonType_e::DUp:
			DUp.push_back(keyVal);
			break;
		case GamePadButtonType_e::DDown:
			DDown.push_back(keyVal);
			break;
		case GamePadButtonType_e::A:
			A.push_back(keyVal);
			break;
		case GamePadButtonType_e::B:
			B.push_back(keyVal);
			break;
		case GamePadButtonType_e::X:
			X.push_back(keyVal);
			break;
		case GamePadButtonType_e::Y:
			Y.push_back(keyVal);
			break;
		case GamePadButtonType_e::LB:
			LB.push_back(keyVal);
			break;
		case GamePadButtonType_e::RB:
			RB.push_back(keyVal);
			break;
		case GamePadButtonType_e::LT:
			LT.push_back(keyVal);
			break;
		case GamePadButtonType_e::RT:
			RT.push_back(keyVal);
			break;
		case GamePadButtonType_e::Back:
			Back.push_back(keyVal);
			break;
		case GamePadButtonType_e::Start:
			Start.push_back(keyVal);
			break;
		case GamePadButtonType_e::LThumb:
			LThumb.push_back(keyVal);
			break;
		case GamePadButtonType_e::RThumb:
			RThumb.push_back(keyVal);
			break;
		default:
			break;
		};
	};

	void EntryKeySet(string presetName, MappingKeyList_t vc, GamePadButtonType_e btnType, KeyType_e keyType)
	{
		auto rslt = _keyTable.find(presetName);

		if (rslt == _keyTable.end())
		{
			return;
		}


		for (auto itr = vc.begin(); itr != vc.end(); itr++)
		{
			KeyMapInfo info;
			info.gamePadKeyType = btnType;
			info.keyType = keyType;
			info.keyVal = *itr;

			rslt->second.push_back(info);
		}
	};

	void SetKeyTableDefaultKeyInfo(string presetName)
	{
		EntryKeySet(presetName, LLeft, GamePadButtonType_e::LLeft, KeyType_e::ORIGIN);
		EntryKeySet(presetName, LRight, GamePadButtonType_e::LRight, KeyType_e::ORIGIN);
		EntryKeySet(presetName, LUp, GamePadButtonType_e::LUp, KeyType_e::ORIGIN);
		EntryKeySet(presetName, LDown, GamePadButtonType_e::LDown, KeyType_e::ORIGIN);
		EntryKeySet(presetName, RLeft, GamePadButtonType_e::RLeft, KeyType_e::ORIGIN);
		EntryKeySet(presetName, RRight, GamePadButtonType_e::RRight, KeyType_e::ORIGIN);
		EntryKeySet(presetName, RUp, GamePadButtonType_e::RUp, KeyType_e::ORIGIN);
		EntryKeySet(presetName, RDown, GamePadButtonType_e::RDown, KeyType_e::ORIGIN);
		EntryKeySet(presetName, DLeft, GamePadButtonType_e::DLeft, KeyType_e::ORIGIN);
		EntryKeySet(presetName, DRight, GamePadButtonType_e::DRight, KeyType_e::ORIGIN);
		EntryKeySet(presetName, DUp, GamePadButtonType_e::DUp, KeyType_e::ORIGIN);
		EntryKeySet(presetName, DDown, GamePadButtonType_e::DDown, KeyType_e::ORIGIN);
		EntryKeySet(presetName, A, GamePadButtonType_e::A, KeyType_e::ORIGIN);
		EntryKeySet(presetName, B, GamePadButtonType_e::B, KeyType_e::ORIGIN);
		EntryKeySet(presetName, X, GamePadButtonType_e::X, KeyType_e::ORIGIN);
		EntryKeySet(presetName, Y, GamePadButtonType_e::Y, KeyType_e::ORIGIN);
		EntryKeySet(presetName, LB, GamePadButtonType_e::LB, KeyType_e::ORIGIN);
		EntryKeySet(presetName, RB, GamePadButtonType_e::RB, KeyType_e::ORIGIN);
		EntryKeySet(presetName, LT, GamePadButtonType_e::LT, KeyType_e::ORIGIN);
		EntryKeySet(presetName, RT, GamePadButtonType_e::RT, KeyType_e::ORIGIN);
		EntryKeySet(presetName, Back, GamePadButtonType_e::Back, KeyType_e::ORIGIN);
		EntryKeySet(presetName, Start, GamePadButtonType_e::Start, KeyType_e::ORIGIN);
		EntryKeySet(presetName, LThumb, GamePadButtonType_e::LThumb, KeyType_e::ORIGIN);
		EntryKeySet(presetName, RThumb, GamePadButtonType_e::RThumb, KeyType_e::ORIGIN);
	};

	//CodeSomnia : ÀúÀåµÈ ÆÄÀÏÀÌ ¾øÀ» °æ¿ì ±âº» Å°·Î ÃÊ±âÈ­
	void SetInitialKey(string presetName) {
		ClearAllKeys();

		_curPresetName = presetName;

		SetToDefaultKeyInfo(GamePadButtonType_e::LLeft, KEY_A);
		SetToDefaultKeyInfo(GamePadButtonType_e::LRight, KEY_D);
		SetToDefaultKeyInfo(GamePadButtonType_e::LUp, KEY_W);
		SetToDefaultKeyInfo(GamePadButtonType_e::LDown, KEY_S);

		SetToDefaultKeyInfo(GamePadButtonType_e::RLeft, KEY_LEFT);
		SetToDefaultKeyInfo(GamePadButtonType_e::RRight, KEY_RIGHT);
		SetToDefaultKeyInfo(GamePadButtonType_e::RUp, KEY_UP);
		SetToDefaultKeyInfo(GamePadButtonType_e::RDown, KEY_DOWN);

		SetToDefaultKeyInfo(GamePadButtonType_e::DLeft, KEY_KP_4);
		SetToDefaultKeyInfo(GamePadButtonType_e::DRight, KEY_KP_6);
		SetToDefaultKeyInfo(GamePadButtonType_e::DUp, KEY_KP_8);
		SetToDefaultKeyInfo(GamePadButtonType_e::DDown, KEY_KP_5);

		SetToDefaultKeyInfo(GamePadButtonType_e::A, KEY_L);
		SetToDefaultKeyInfo(GamePadButtonType_e::B, KEY_O);
		SetToDefaultKeyInfo(GamePadButtonType_e::X, KEY_K);
		SetToDefaultKeyInfo(GamePadButtonType_e::Y, KEY_I);
		SetToDefaultKeyInfo(GamePadButtonType_e::LB, KEY_Q);
		SetToDefaultKeyInfo(GamePadButtonType_e::RB, KEY_P);
		SetToDefaultKeyInfo(GamePadButtonType_e::LT, KEY_F);
		SetToDefaultKeyInfo(GamePadButtonType_e::RT, KEY_J);

		SetToDefaultKeyInfo(GamePadButtonType_e::Back, KEY_BACKSPACE);
		SetToDefaultKeyInfo(GamePadButtonType_e::Start, KEY_ENTER);
		SetToDefaultKeyInfo(GamePadButtonType_e::LThumb, KEY_C);
		SetToDefaultKeyInfo(GamePadButtonType_e::RThumb, KEY_M);

		_keyTable[presetName] = KeySet_t();
		SetKeyTableDefaultKeyInfo(presetName);
	}

	bool isLLeft(uint32_t userID, uint16_t val){
		
		// Find the profile
		KeyboardProfile* profile = findProfile(userID);
		if (profile != nullptr) {
			return profile->keyMap[0] == val;
		} else {
			return profiles[0].keyMap[0] == val;
		};

	};

	bool isLRight(uint32_t userID, uint16_t val) {
		KeyboardProfile* profile = findProfile(userID);
		if (profile != nullptr) {
			return profile->keyMap[1] == val;
		} else {
			return profiles[0].keyMap[1] == val;
		};
	};

	bool isLUp(uint32_t userID, uint16_t val) {
		KeyboardProfile* profile = findProfile(userID);
		if (profile != nullptr) {
			return profile->keyMap[2] == val;
		} else {
			return profiles[0].keyMap[2] == val;
		};
	};

	bool isLDown(uint32_t userID, uint16_t val) {
		KeyboardProfile* profile = findProfile(userID);
		if (profile != nullptr) {
			return profile->keyMap[3] == val;
		} else {
			return profiles[0].keyMap[3] == val;
		};
	};

	bool isRLeft(uint32_t userID, uint16_t val) {
		KeyboardProfile* profile = findProfile(userID);
		if (profile != nullptr) {
			return profile->keyMap[4] == val;
		} else {
			return profiles[0].keyMap[4] == val;
		};
	};

	bool isRRight(uint32_t userID, uint16_t val) {
		KeyboardProfile* profile = findProfile(userID);
		if (profile != nullptr) {
			return profile->keyMap[5] == val;
		} else {
			return profiles[0].keyMap[5] == val;
		};
	};

	bool isRUp(uint32_t userID, uint16_t val) {
		KeyboardProfile* profile = findProfile(userID);
		if (profile != nullptr) {
			return profile->keyMap[6] == val;
		} else {
			return profiles[0].keyMap[6] == val;
		};
	};

	bool isRDown(uint32_t userID, uint16_t val) {
		KeyboardProfile* profile = findProfile(userID);
		if (profile != nullptr) {
			return profile->keyMap[7] == val;
		} else {
			return profiles[0].keyMap[7] == val;
		};
	};

	bool isDLeft(uint32_t userID, uint16_t val) {
		KeyboardProfile* profile = findProfile(userID);
		if (profile != nullptr) {
			return profile->keyMap[8] == val;
		} else {
			return profiles[0].keyMap[8] == val;
		};
	};

	bool isDRight(uint32_t userID, uint16_t val) {
		KeyboardProfile* profile = findProfile(userID);
		if (profile != nullptr) {
			return profile->keyMap[9] == val;
		} else {
			return profiles[0].keyMap[9] == val;
		};
	};

	bool isDUp(uint32_t userID, uint16_t val) {
		KeyboardProfile* profile = findProfile(userID);
		if (profile != nullptr) {
			return profile->keyMap[10] == val;
		} else {
			return profiles[0].keyMap[10] == val;
		};
	};

	bool isDDown(uint32_t userID, uint16_t val) {
		KeyboardProfile* profile = findProfile(userID);
		if (profile != nullptr) {
			return profile->keyMap[11] == val;
		} else {
			return profiles[0].keyMap[11] == val;
		};
	};

	bool isA(uint32_t userID, uint16_t val) {
		KeyboardProfile* profile = findProfile(userID);
		if (profile != nullptr) {
			return profile->keyMap[12] == val;
		} else {
			return profiles[0].keyMap[12] == val;
		};
	};

	bool isB(uint32_t userID, uint16_t val) {
		KeyboardProfile* profile = findProfile(userID);
		if (profile != nullptr) {
			return profile->keyMap[13] == val;
		} else {
			return profiles[0].keyMap[13] == val;
		};
	};

	bool isX(uint32_t userID, uint16_t val) {
		KeyboardProfile* profile = findProfile(userID);
		if (profile != nullptr) {
			return profile->keyMap[14] == val;
		} else {
			return profiles[0].keyMap[14] == val;
		};
	};

	bool isY(uint32_t userID, uint16_t val) {
		KeyboardProfile* profile = findProfile(userID);
		if (profile != nullptr) {
			return profile->keyMap[15] == val;
		} else {
			return profiles[0].keyMap[15] == val;
		};
	};

	bool isLB(uint32_t userID, uint16_t val) {
		KeyboardProfile* profile = findProfile(userID);
		if (profile != nullptr) {
			return profile->keyMap[16] == val;
		} else {
			return profiles[0].keyMap[16] == val;
		};
	};

	bool isRB(uint32_t userID, uint16_t val) {
		KeyboardProfile* profile = findProfile(userID);
		if (profile != nullptr) {
			return profile->keyMap[17] == val;
		} else {
			return profiles[0].keyMap[17] == val;
		};
	};

	bool isLT(uint32_t userID, uint16_t val) {
		KeyboardProfile* profile = findProfile(userID);
		if (profile != nullptr) {
			return profile->keyMap[18] == val;
		} else {
			return profiles[0].keyMap[18] == val;
		};
	};

	bool isRT(uint32_t userID, uint16_t val) {
		KeyboardProfile* profile = findProfile(userID);
		if (profile != nullptr) {
			return profile->keyMap[19] == val;
		} else {
			return profiles[0].keyMap[19] == val;
		};
	};

	bool isBack(uint32_t userID, uint16_t val) {
		KeyboardProfile* profile = findProfile(userID);
		if (profile != nullptr) {
			return profile->keyMap[20] == val;
		} else {
			return profiles[0].keyMap[20] == val;
		};
	};

	bool isStart(uint32_t userID, uint16_t val) {
		KeyboardProfile* profile = findProfile(userID);
		if (profile != nullptr) {
			return profile->keyMap[21] == val;
		} else {
			return profiles[0].keyMap[21] == val;
		};
	};

	bool isLThumb(uint32_t userID, uint16_t val) {
		KeyboardProfile* profile = findProfile(userID);
		if (profile != nullptr) {
			return profile->keyMap[22] == val;
		} else {
			return profiles[0].keyMap[22] == val;
		};
	};

	bool isRThumb(uint32_t userID, uint16_t val) {
		KeyboardProfile* profile = findProfile(userID);
		if (profile != nullptr) {
			return profile->keyMap[23] == val;
		} else {
			return profiles[0].keyMap[23] == val;
		};
	};

	inline uint16_t getDefaultLLeft() { return LLeft.size() > 0 ? LLeft[0] : KEY_A; };
	inline uint16_t getDefaultLRight() { return LRight.size() > 0 ? LRight[0] : KEY_D; };
	inline uint16_t getDefaultLDown() { return LDown.size() > 0 ? LDown[0] : KEY_W; };
	inline uint16_t getDefaultLUp() { return LUp.size() > 0 ? LUp[0] : KEY_S; };

	inline uint16_t getDefaultRLeft() { return RLeft.size() > 0 ? RLeft[0] : KEY_LEFT; };
	inline uint16_t getDefaultRRight() { return RRight.size() > 0 ? RRight[0] : KEY_RIGHT; };
	inline uint16_t getDefaultRUp() { return RUp.size() > 0 ? RUp[0] : KEY_UP; };
	inline uint16_t getDefaultRDown() { return RDown.size() > 0 ? RDown[0] : KEY_DOWN; };

	inline uint16_t getDefaultDLeft() { return DLeft.size() > 0 ? DLeft[0] : KEY_KP_4; };
	inline uint16_t getDefaultDRight() { return DRight.size() > 0 ? DRight[0] : KEY_KP_6; };
	inline uint16_t getDefaultDUp() { return DUp.size() > 0 ? DUp[0] : KEY_KP_8; };
	inline uint16_t getDefaultDDown() { return DDown.size() > 0 ? DDown[0] : KEY_KP_5; };

	inline uint16_t getDefaultA() { return A.size() > 0 ? A[0] : KEY_L; };
	inline uint16_t getDefaultB() { return B.size() > 0 ? B[0] : KEY_O; };
	inline uint16_t getDefaultX() { return X.size() > 0 ? X[0] : KEY_K; };
	inline uint16_t getDefaultY() { return Y.size() > 0 ? Y[0] : KEY_I; };

	inline uint16_t getDefaultLB() { return LB.size() > 0 ? LB[0] : KEY_Q; };
	inline uint16_t getDefaultRB() { return RB.size() > 0 ? RB[0] : KEY_P; };
	inline uint16_t getDefaultLT() { return LT.size() > 0 ? LT[0] : KEY_F; };
	inline uint16_t getDefaultRT() { return RT.size() > 0 ? RT[0] : KEY_J; };

	inline uint16_t getDefaultBack() { return Back.size() > 0 ? Back[0] : KEY_BACKSPACE; };
	inline uint16_t getDefaultStart() { return Start.size() > 0 ? Start[0] : KEY_ENTER; };
	inline uint16_t getDefaultLThumb() { return LThumb.size() > 0 ? LThumb[0] : KEY_C; };
	inline uint16_t getDefaultRThumb() { return RThumb.size() > 0 ? RThumb[0] : KEY_M; };


	void ClearAllKeys()
	{//°ÔÀÓÆÐµå¿¡ ¸ÅÇÎµÇ¾îÀÖ´Â ¸ðµç Å°Á¤º¸¸¦ Å¬¸®¾î
		LLeft.clear();
		LRight.clear();
		LUp.clear();
		LDown.clear();

		RLeft.clear();
		RRight.clear();
		RUp.clear();
		RDown.clear();

		DLeft.clear();
		DRight.clear();
		DUp.clear();
		DDown.clear();

		A.clear();
		B.clear();
		X.clear();
		Y.clear();

		LB.clear();
		RB.clear();
		LT.clear();
		RT.clear();

		Back.clear();
		Start.clear();

		LThumb.clear();
		RThumb.clear();

	};

	// Left Stick
	MappingKeyList_t LLeft;
	MappingKeyList_t LRight;
	MappingKeyList_t LUp;
	MappingKeyList_t LDown;

	// Right Stick
	MappingKeyList_t RLeft;
	MappingKeyList_t RRight;
	MappingKeyList_t RUp;
	MappingKeyList_t RDown;

	// DPad
	MappingKeyList_t DLeft;
	MappingKeyList_t DRight;
	MappingKeyList_t DUp;
	MappingKeyList_t DDown;

	// Face Buttons
	MappingKeyList_t A;
	MappingKeyList_t B;
	MappingKeyList_t X;
	MappingKeyList_t Y;

	// Top
	MappingKeyList_t LB;
	MappingKeyList_t RB;
	MappingKeyList_t LT;
	MappingKeyList_t RT;

	// Menu
	MappingKeyList_t Back;
	MappingKeyList_t Start;

	// Stick Buttons
	MappingKeyList_t LThumb;
	MappingKeyList_t RThumb;

	vector<KeyMapInfo> _customKeyList;

	string _curPresetName;
	uint32_t _curPresetID = 0;

	map<string, KeySet_t> _keyTable;
	//¼³Á¤ÇÑ Å°¸ÅÇÎ Á¤º¸¸¦ ¸ðµÎ ¼ÒÀ¯ÇÑ Å×ÀÌºí.


	string AddNewPreset() {
		vector<KeyMapInfo> keySet;
		string presetName;
		int idx = 0;

		while (true)
		{
			presetName = "New Preset_" + to_string(_keyTable.size() + idx);
			auto item = _keyTable.find(presetName);

			if (item == _keyTable.end())
				break;
			idx++;
		}

		_keyTable[presetName] = keySet;
		_curPresetName = presetName;
		_customKeyList.clear();

		ClearAllKeys();
		SetInitialKey(presetName);
		return presetName;
	};

	bool UpdateCurrentStateToTable() {
		//ÇöÀç ÇÁ¸®¼Â Á¤º¸¸¦ Å°Å×ÀÌºí¿¡ ÀÔ·ÂÇÕ´Ï´Ù.
		auto rslt = _keyTable.find(_curPresetName);

		if (rslt == _keyTable.end())
		{
			return false;
		}

		rslt->second.clear();
		SetKeyTableDefaultKeyInfo(_curPresetName);

		for (auto itr = _customKeyList.begin(); itr != _customKeyList.end(); itr++)
		{
			rslt->second.push_back(*itr);
		}

	};

	bool RemoveCurPreset()
	{//ÇöÀç ¼³Á¤µÇ¾îÀÖ´Â ÇÁ¸®¼ÂÀ» »èÁ¦
		auto rslt = _keyTable.find(_curPresetName);

		if (rslt == _keyTable.end() || _keyTable.size() <= 1)
		{//ÃÖ¼ÒÇÑ ÇÑ°³ÀÇ ÇÁ¸®¼ÂÀº Á¸ÀçÇØ¾ßÇÏ¹Ç·Î ÇØ´ç ÇÁ¸®¼ÂÀÌ Á¸ÀçÇÏÁö ¾Ê°Å³ª, Å°¸ÊÅ×ÀÌºí¿¡ ÃÖ¼Ò ÇÑ°³ÀÌ»ó ³²¾ÆÀÖÀ» ¼ö ÀÖµµ·Ï ¿¹¿ÜÃ³¸®
			return false;
		}

		rslt->second.clear();
		_keyTable.erase(rslt);

		auto firstItem = _keyTable.begin();

		LoadPreset(firstItem->first);

		return true;
	}

	bool isAvailablePresetName(string newPresetName)
	{
		//»õ·Î¿î ÇÁ¸®¼ÂÀÌ¸§À» »ç¿ë ÇÒ ¼ö ÀÖ´ÂÁö È®ÀÎ.

		auto rslt = _keyTable.find(newPresetName);

		if (rslt == _keyTable.end())
		{
			return true;
		}

		return false;
	}

	bool UpdatePresetName(string newPresetName, string oldPresetName)
	{//ÇÁ¸®¼Â ÀÌ¸§À» ¾÷µ¥ÀÌÆ®ÇÕ´Ï´Ù.
		auto rslt = _keyTable.find(oldPresetName);

		if (rslt == _keyTable.end())
		{//ÀÌÀü ÀÌ¸§ÀÇ ÇÁ¸®¼ÂÀÌ Å°Å×ÀÌºí¿¡ ¾ø´Â°æ¿ì ¾÷µ¥ÀÌÆ®ÇÏÁö ÇÒ¼ö ¾ø´Â ¿¹¿ÜÃ³¸®.
			return false;
		}

		vector<KeyMapInfo> keySet = _keyTable[oldPresetName];
		_keyTable.erase(rslt);

		_keyTable[newPresetName] = keySet;
		_curPresetName = newPresetName;

		return true;
	};

	void LoadPreset(string presetName)
	{ //ÇÁ¸®¼Â ÀÌ¸§À» ÅëÇØ ÇØ´çÀÌ¸§ÀÇ ÇÁ¸®¼Â Á¤º¸¸¦ ºÒ·¯¿Í Àû¿ëÇÕ´Ï´Ù.
		if (_keyTable.find(presetName) == _keyTable.end())
		{
			return;
		}

		ClearAllKeys();
		_customKeyList.clear();

		vector<KeyMapInfo> keySet = _keyTable[presetName];
		_curPresetName = presetName;

		for (auto itr = keySet.begin(); itr != keySet.end(); itr++)
		{
			SetToDefaultKeyInfo((GamePadButtonType_e)itr->gamePadKeyType, itr->keyVal);

			if (itr->keyType == KeyType_e::CUSTOM)
			{
				_customKeyList.push_back(*itr);
			}
		}
	};

	bool SaveKeyMap()
	{ //¼³Á¤ÇÑ Å°¸ÅÇÎÀ» ÆÄÀÏ·Î ÀúÀåÇÕ´Ï´Ù
		string dirPath = KeyboardMapsUtil::getKeyMapSavePath();

		if (!dirPath.empty())
		{
			string filePath = dirPath + +"ParsecSodaKeyMap.json";
			MTY_JSON* json = MTY_JSONObjCreate();

			MTY_JSONObjSetString(json, "CurrentPresetName", _curPresetName.c_str());

			MTY_JSON* presetLst = MTY_JSONArrayCreate();

			for (auto itr = _keyTable.begin(); itr != _keyTable.end(); itr++)
			{
				MTY_JSON* presetInfo = MTY_JSONObjCreate();

				MTY_JSONObjSetString(presetInfo, "PresetName", itr->first.c_str());
				MTY_JSON* keyLst = MTY_JSONArrayCreate();

				for (auto keyItemItr = itr->second.begin(); keyItemItr != itr->second.end(); keyItemItr++)
				{
					MTY_JSON* keyInfo = MTY_JSONObjCreate();

					MTY_JSONObjSetInt(keyInfo, "GamePadKeyType", keyItemItr->gamePadKeyType);
					MTY_JSONObjSetInt(keyInfo, "KeyValue", keyItemItr->keyVal);
					MTY_JSONObjSetInt(keyInfo, "KeyType", keyItemItr->keyType);

					MTY_JSONArrayAppendItem(keyLst, keyInfo);
				}

				MTY_JSONObjSetItem(presetInfo, "KeySet", keyLst);
				MTY_JSONArrayAppendItem(presetLst, presetInfo);
			}
			//
			MTY_JSONObjSetItem(json, "PresetList", presetLst);

			MTY_JSONWriteFile(filePath.c_str(), json);
			MTY_JSONDestroy(&json);

			return true;

		}

		return false;

	};
};
//-- CodeSomnia Modified End--