#include "GuestStateStore.h"

#include "matoya.h"
#include "PathHelper.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

GuestStateStore& GuestStateStore::instance() {
    static GuestStateStore s_instance;
    return s_instance;
}

void GuestStateStore::load() {
    _guestInputPermissions.clear();

    const std::string filePath = getFilePath();
    if (!MTY_FileExists(filePath.c_str())) {
        return;
    }

    size_t size = 0;
    void* data = MTY_ReadFile(filePath.c_str(), &size);
    if (data == nullptr || size == 0) {
        return;
    }

    try {
        json j = json::parse(reinterpret_cast<const char*>(data), reinterpret_cast<const char*>(data) + size);
        if (!j.contains("GuestInputPermissions") || !j["GuestInputPermissions"].is_array()) {
            MTY_Free(data);
            return;
        }

        for (const json& entry : j["GuestInputPermissions"]) {
            GuestPermissionState state;
            state.userID = entry.value("userID", 0u);
            state.gamepad = entry.value("gamepad", true);
            state.keyboard = entry.value("keyboard", true);
            state.mouse = entry.value("mouse", true);

            if (state.userID != 0) {
                _guestInputPermissions.push_back(state);
            }
        }
    }
    catch (const json::exception&) {
    }

    MTY_Free(data);
}

void GuestStateStore::save() const {
    json j;

    std::vector<json> permissions;
    for (const GuestPermissionState& state : _guestInputPermissions) {
        permissions.push_back({
            {"userID", state.userID},
            {"gamepad", state.gamepad},
            {"keyboard", state.keyboard},
            {"mouse", state.mouse}
        });
    }

    j["GuestInputPermissions"] = permissions;

    const std::string filePath = getFilePath();
    const std::string payload = j.dump(4);
    MTY_WriteTextFile(filePath.c_str(), "%s", payload.c_str());
}

void GuestStateStore::setGuestInputPermissions(uint32_t userID, bool gamepad, bool keyboard, bool mouse) {
    for (GuestPermissionState& state : _guestInputPermissions) {
        if (state.userID == userID) {
            state.gamepad = gamepad;
            state.keyboard = keyboard;
            state.mouse = mouse;
            return;
        }
    }

    GuestPermissionState state;
    state.userID = userID;
    state.gamepad = gamepad;
    state.keyboard = keyboard;
    state.mouse = mouse;
    _guestInputPermissions.push_back(state);
}

bool GuestStateStore::tryGetGuestInputPermissions(uint32_t userID, GuestPermissionState& state) const {
    for (const GuestPermissionState& candidate : _guestInputPermissions) {
        if (candidate.userID == userID) {
            state = candidate;
            return true;
        }
    }

    return false;
}

std::string GuestStateStore::getFilePath() const {
    return PathHelper::GetConfigPath() + "guest_state.json";
}
