#pragma once
#include "KeyboardMapWidget.h"
#include "../core/GamepadClient.h"
#include <assert.h>

vector<pair<string, string>> KeyboardMapWidget::_keyValues;
bool KeyboardMapWidget::_keyNamesInitialized = false;

void KeyboardMapWidget::initKeyValues() {
    if (_keyNamesInitialized) return;

    int allowedKeyLen = sizeof(_allowedSettionKeys) / sizeof(uint16_t);
    _keyValues.reserve(allowedKeyLen);

    for (int i = 0; i < allowedKeyLen; i++) {
        uint16_t key = _allowedSettionKeys[i];
        string keyStr = to_string(key);
        string keyName = KeyboardMapsUtil::getKeyToString(key);
        _keyValues.push_back(make_pair(keyStr, keyName));
    }

    _keyNamesInitialized = true;
}

KeyboardMapWidget::KeyboardMapWidget(Hosting& hosting)
	: _hosting(hosting)
{
    initKeyValues();
    InitializeKeyMapSetting(hosting);
}

inline void KeyboardMapWidget::resetPresetName(const char* str, int len)
{
    memset(_presetNameBuffer, 0, PRESET_NAME_LEN);
    memcpy(_presetNameBuffer, str, len);
}

void KeyboardMapWidget::InitializeKeyMapSetting(Hosting& hosting)
{
    KeyboardMap& hostingKeyMap = GamepadClient::instance.getKeyMap();

    _lLeft = hostingKeyMap.getDefaultLLeft();
    _lRight = hostingKeyMap.getDefaultLRight();
    _lUp = hostingKeyMap.getDefaultLUp();
    _lDown = hostingKeyMap.getDefaultLDown();

    _rLeft = hostingKeyMap.getDefaultRLeft();
    _rRight = hostingKeyMap.getDefaultRRight();
    _rUp = hostingKeyMap.getDefaultRUp();
    _rDown = hostingKeyMap.getDefaultRDown();

    _dLeft = hostingKeyMap.getDefaultDLeft();
    _dRight = hostingKeyMap.getDefaultDRight();
    _dUp = hostingKeyMap.getDefaultDUp();
    _dDown = hostingKeyMap.getDefaultDDown();

    _a = hostingKeyMap.getDefaultA();
    _b = hostingKeyMap.getDefaultB();
    _x = hostingKeyMap.getDefaultX();
    _y = hostingKeyMap.getDefaultY();

    _lB = hostingKeyMap.getDefaultLB();
    _rB = hostingKeyMap.getDefaultRB();
    _lT = hostingKeyMap.getDefaultLT();
    _rT = hostingKeyMap.getDefaultRT();

    _back = hostingKeyMap.getDefaultBack();
    _start = hostingKeyMap.getDefaultStart();

    _lThumb = hostingKeyMap.getDefaultLThumb();
    _rThumb = hostingKeyMap.getDefaultRThumb();

    memcpy(_presetNameBuffer, hostingKeyMap._curPresetName.c_str(), hostingKeyMap._curPresetName.length());
}

void KeyboardMapWidget::ApplyKeyMap()
{
    // CodeSomnia : ¼³Á¤ÇÑ Å°¸ÊÀ» Hosting¿¡¼­ Àû¿ë µÉ¼ö ÀÖµµ·Ï ÇÑ´Ù
    //        
    KeyboardMap& hostingKeyMap = GamepadClient::instance.getKeyMap();
    hostingKeyMap.ClearAllKeys();

    hostingKeyMap.SetToDefaultKeyInfo(GamePadButtonType_e::LLeft , _lLeft);
    hostingKeyMap.SetToDefaultKeyInfo(GamePadButtonType_e::LRight, _lRight);
    hostingKeyMap.SetToDefaultKeyInfo(GamePadButtonType_e::LUp, _lUp);
    hostingKeyMap.SetToDefaultKeyInfo(GamePadButtonType_e::LDown, _lDown);

    hostingKeyMap.SetToDefaultKeyInfo(GamePadButtonType_e::RLeft, _rLeft);
    hostingKeyMap.SetToDefaultKeyInfo(GamePadButtonType_e::RRight, _rRight);
    hostingKeyMap.SetToDefaultKeyInfo(GamePadButtonType_e::RUp, _rUp);
    hostingKeyMap.SetToDefaultKeyInfo(GamePadButtonType_e::RDown, _rDown);

    hostingKeyMap.SetToDefaultKeyInfo(GamePadButtonType_e::DLeft, _dLeft);
    hostingKeyMap.SetToDefaultKeyInfo(GamePadButtonType_e::DRight, _dRight);
    hostingKeyMap.SetToDefaultKeyInfo(GamePadButtonType_e::DUp, _dUp);
    hostingKeyMap.SetToDefaultKeyInfo(GamePadButtonType_e::DDown, _dDown);

    hostingKeyMap.SetToDefaultKeyInfo(GamePadButtonType_e::A, _a);
    hostingKeyMap.SetToDefaultKeyInfo(GamePadButtonType_e::B, _b);
    hostingKeyMap.SetToDefaultKeyInfo(GamePadButtonType_e::X, _x);
    hostingKeyMap.SetToDefaultKeyInfo(GamePadButtonType_e::Y, _y);

    hostingKeyMap.SetToDefaultKeyInfo(GamePadButtonType_e::LB, _lB);
    hostingKeyMap.SetToDefaultKeyInfo(GamePadButtonType_e::RB, _rB);
    hostingKeyMap.SetToDefaultKeyInfo(GamePadButtonType_e::LT, _lT);
    hostingKeyMap.SetToDefaultKeyInfo(GamePadButtonType_e::RT, _rT);

    hostingKeyMap.SetToDefaultKeyInfo(GamePadButtonType_e::Back, _back);
    hostingKeyMap.SetToDefaultKeyInfo(GamePadButtonType_e::Start, _start);

    hostingKeyMap.SetToDefaultKeyInfo(GamePadButtonType_e::LThumb, _lThumb);
    hostingKeyMap.SetToDefaultKeyInfo(GamePadButtonType_e::RThumb, _rThumb);

    hostingKeyMap.UpdateCurrentStateToTable();

    for (auto itr = hostingKeyMap._customKeyList.begin();
        itr < hostingKeyMap._customKeyList.end(); itr++)
    {
        hostingKeyMap.SetToDefaultKeyInfo((GamePadButtonType_e)itr->gamePadKeyType, itr->keyVal);
    }

}
void KeyboardMapWidget::AddJsonKeyMapItem(MTY_JSON* json, GamePadButtonType_e gamePadKeyType, uint16_t keyVal, int32_t keyType)
{
    MTY_JSON* item = MTY_JSONObjCreate();
    MTY_JSONObjSetUInt(item, "GamePadKeyType", gamePadKeyType);
    MTY_JSONObjSetUInt(item, "KeyValue", keyVal);
    MTY_JSONObjSetUInt(item, "KeyType", keyType);

    MTY_JSONArrayAppendItem(json,item);
}

bool KeyboardMapWidget::SaveKeyFile()
{
    return GamepadClient::instance.getKeyMap().SaveKeyMap();
}

/*
    CodeSomnia : Ä¿½ºÅÒÅ° UI¸¦ µå·ÎÀ× Ä¿½ºÅÒ Å°¸¦ ¼³Á¤ÇÏ±â À§ÇÑ ÄÁÆ®·ÑÀ» ÇÑ¶óÀÎ¾¿ »ý¼ºÇÏ´Â ÇÔ¼ö
*/ 
bool KeyboardMapWidget::drawCustomKey(int32_t& gamePadKeyType, uint16_t& key,int idx) {
    
    AppFonts::pushInput();
    bool isNeedDelete = false;
    int allowedKeyLen = sizeof(_allowedSettionKeys) / sizeof(uint16_t);
    
    string keyLabelName = KeyboardMapsUtil::getGamePadKeyTypeToString((GamePadButtonType_e)gamePadKeyType);
    string labelName = "##" + keyLabelName +"_Custom_KeyName"+ to_string(idx);
    
    ImGui::SetNextItemWidth(COLUMN_1_SIZE+13); 
    // Ã¹¹øÂ° ¿­ÀÇ ±âº»»çÀÌÁî¸¦ Àû¿ëÇÏ±â À§ÇØ ±×´ÙÀ½¿¡ ¿À´Â ÄÞº¸ ¹Ú½ºÀÇ »çÀÌÁî¸¦ °áÁ¤
    
    if (ImGui::BeginCombo(labelName.c_str(), keyLabelName.c_str()))
    {
        for (int idx = 0; idx < GAMEPAD_KEY_COUNT; idx++)
        {
            bool isSelected = keyLabelName == _allowedCustomKeys[idx].name;

            if (ImGui::Selectable(_allowedCustomKeys[idx].name.c_str(), isSelected))
            {
                //ÄÞº¸ ¹Ú½º°¡ Expand µÉ¶§ Ç¥ÃâµÇ´Â Ç×¸ñµå¸¦ DrawingÇÑ´Ù.
                //¸¸¾à Ç×¸ñÀÌ ¼±ÅÃµÇ¸é ±âº» ÄÞº¸¹Ú½º¿¡ ¼±ÅÃµÈ Ç×¸ñÀÌ ´ëÇ¥ÀûÀ¸·Î Ç¥Ãâ µÉ¼ö ÀÖµµ·ÏÇÑ´Ù.
                keyLabelName = _allowedCustomKeys[idx].name;
                gamePadKeyType = _allowedCustomKeys[idx].buttonType;
            }

            if (isSelected)
            {
                //¼±ÅÃµÈ Ç×¸ñ¿¡ ±âº» Æ÷Ä¿½º¸¦ ¼³Á¤ÇÑ´Ù.
                ImGui::SetItemDefaultFocus();
            }
        }

        ImGui::EndCombo();
        //ÄÞº¸¹Ú½º ±×¸®±â¸¦ ³¡³½´Ù
    }

    ImGui::SameLine();
    // °ÔÀÓÅ°ÆÐµå Á¾·ù ÄÞº¸¹Ú½º¿Í Å°º¸µå ¸ÅÇÎÅ° ¼³Á¤ ÄÞº¸¹Ú½º¸¦ °°Àº ¶óÀÎ¿¡ µå·ÎÀ× ÇÒ ¼ö ÀÖµµ·Ï ÇÑ´Ù.
   
    string curKeyName = KeyboardMapsUtil::getKeyToString(key);
    string keyValName = "##" + keyLabelName + "_Custom_Val" + to_string(idx);
    ImGui::SetNextItemWidth(150);

    if (ImGui::BeginCombo(keyValName.c_str(), curKeyName.c_str()))
    {
        //¼³Á¤ÇÒ¼ö ÀÖ´Â Å°º¸µåµéÀ» ÄÞº¸¹Ú½º¿¡ Ãß°¡ÇÑ´Ù. ±âº»ÀûÀÎ ·ÎÁ÷Àº °ÔÀÓÅ°ÆÐµå ÄÞº¸¹Ú½º ±×¸®±â¿Í °°´Ù.
        for (int idx = 0; idx < allowedKeyLen; idx++)
        {
            bool isSelected = key == _allowedSettionKeys[idx];

            string itemKeyName = KeyboardMapsUtil::getKeyToString(_allowedSettionKeys[idx]);

            if (ImGui::Selectable(itemKeyName.c_str(), isSelected))
            {
                curKeyName = itemKeyName;
                key = _allowedSettionKeys[idx];
            }

            if (isSelected)
            {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }

    ImGui::SameLine();
    string btnLabel = "##Remove_CustomKey" + std::to_string(idx);
    //Ä¿½ºÅÒ Å° Ç×¸ñÀ» »èÁ¦ ÇÒ ¼ö ÀÖµµ·Ï ¹öÆ°À» Ãß°¡ÇÑ´Ù.
    if (IconButton::renderWithLabel(btnLabel.c_str(), AppIcons::trash, AppColors::primary,ImVec2(20.0,20.0))) {
        isNeedDelete = true;
    }

    AppFonts::pop();

    return isNeedDelete;

}

bool KeyboardMapWidget::render(bool &showWindow) {
    KeyboardMap& hostingKeyMap = GamepadClient::instance.getKeyMap();

    startWidget("Keyboard Mapper", showWindow, 520, 720, 520, 420);
    startBody(true);
        drawPresetList(hostingKeyMap._curPresetName);

        ImGui::Separator();
        ImGui::Dummy(ImVec2(0.0f, 10.0f));

        drawKeySetLine("LLeft", _lLeft);
        drawKeySetLine("LRight", _lRight);
        drawKeySetLine("LUp", _lUp);
        drawKeySetLine("LDown", _lDown);

        drawKeySetLine("RLeft", _rLeft);
        drawKeySetLine("RRight", _rRight);
        drawKeySetLine("RUp", _rUp);
        drawKeySetLine("RDown", _rDown);

        drawKeySetLine("DLeft", _dLeft);
        drawKeySetLine("DRight", _dRight);
        drawKeySetLine("DUp", _dUp);
        drawKeySetLine("DDown", _dDown);

        drawKeySetLine("A", _a);
        drawKeySetLine("B", _b);
        drawKeySetLine("X", _x);
        drawKeySetLine("Y", _y);

        drawKeySetLine("LB", _lB);
        drawKeySetLine("RB", _rB);
        drawKeySetLine("LT", _lT);
        drawKeySetLine("RT", _rT);

        drawKeySetLine("Back", _back);
        drawKeySetLine("Start", _start);

        drawKeySetLine("LThumb", _lThumb);
        drawKeySetLine("RThumb", _rThumb);

        ImGui::Dummy(ImVec2(0, 10));

    endBody();
    startFooter();

        if (elBtnSecondary("Reset to Default")) {
            hostingKeyMap.resetProfile(hostingKeyMap._curPresetID);
        }

        // ImGui::SameLine();
        // if (elBtn("Add Preset")) {
        // }

    endFooter();
	return true;
}

string KeyboardMapWidget::drawPresetList(string currentPresetName)
{
    KeyboardMap& hostingKeyMap = GamepadClient::instance.getKeyMap();
    
    vector<pair<string, string>> presetItems;
    for (auto& profile : hostingKeyMap.profiles) {
        presetItems.push_back(make_pair(profile.name, std::to_string(profile.userID) + ":" + profile.name));
    }

    string selectedPreset = hostingKeyMap._curPresetName;

    ImGui::PushStyleColor(ImGuiCol_Border, AppColors::formInputBorder);
    ImGui::PushStyleColor(ImGuiCol_Button, AppColors::buttonPrimary);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, AppColors::buttonPrimaryActive);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, AppColors::buttonPrimaryHovered);
    
    if (elSelect("Preset", presetItems, selectedPreset, "Select a Preset")) {
        hostingKeyMap._curPresetName = selectedPreset;
        for (auto& profile : hostingKeyMap.profiles) {
            if (profile.name == selectedPreset) {
                hostingKeyMap._curPresetID = profile.userID;
                break;
            }
        }
    }

    ImGui::PopStyleColor(4);

    if (hostingKeyMap._curPresetID != 0) {
        ImGui::SameLine();
        if (IconButton::render(AppIcons::trash, AppColors::primary, ImVec2(20.0, 20.0))) {
            uint32_t curPresetID = hostingKeyMap._curPresetID;
            hostingKeyMap._curPresetID = 0;
            hostingKeyMap._curPresetName = "Default";
            hostingKeyMap.deleteProfile(curPresetID);
        }
    }

    return selectedPreset;
}


void KeyboardMapWidget::UpdatePresetName(string newName, string oldName) {
    //PresetNameÀ» º¯°æÇÑ´Ù ÇÏÁö¸¸ ±âÁ¸¿¡ °°Àº ÀÌ¸§À» °®°í ÀÖ°Å³ª, ºó°ø¹éÀ¸·Î¸¸ ÀÌ·ïÁø ÀÌ¸§Àº Çã¿ëÇÏÁö ¾Ê´Â´Ù
    KeyboardMap& hostingKeyMap = GamepadClient::instance.getKeyMap();

    if (newName.empty())
        return;

    if (hostingKeyMap.isAvailablePresetName(newName))
    {
        hostingKeyMap.UpdatePresetName(newName, oldName);

        resetPresetName(newName.c_str(), newName.length());
        //Àû¿ëµÈ ÀÌ¸§ÀÌ¹Ç·Î »õ·Î¿î ÀÌ¸§À¸·Î ¿øº¹ÇÑ´Ù
    }
    else
    {
        resetPresetName(oldName.c_str(), oldName.length());
        //ÀÌ¿ëÇÒ¼ö ¾ø´Â ÀÌ¸§ÀÌ¹Ç·Î ÀÌÀü ÀÌ¸§À¸·Î ¿øº¹ÇÑ´Ù
    }

}

void KeyboardMapWidget::drawKeySetLine(string keyName, uint16_t& key)
{
    AppFonts::pushInput();

    KeyboardMap& hostingKeyMap = GamepadClient::instance.getKeyMap();
    string selectedKey = to_string(hostingKeyMap.getKeyForButton(keyName));
    
    ImGui::PushStyleColor(ImGuiCol_Border, AppColors::formInputBorder);
    ImGui::PushStyleColor(ImGuiCol_Button, AppColors::buttonPrimary);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, AppColors::buttonPrimaryActive);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, AppColors::buttonPrimaryHovered);
    
    if (elSelect(keyName, _keyValues, selectedKey, "Select a key"))
    {
        hostingKeyMap.setButton(keyName, (uint16_t)stoul(selectedKey));
        hostingKeyMap.saveProfile(hostingKeyMap._curPresetID);
    }

    ImGui::PopStyleColor(4);

    AppFonts::pop();
}