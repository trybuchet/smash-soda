#pragma once

#include <cstdint>
#include <string>
#include <vector>

class GuestStateStore {
public:
    class GuestPermissionState {
    public:
        uint32_t userID = 0;
        bool gamepad = true;
        bool keyboard = true;
        bool mouse = true;
    };

    static GuestStateStore& instance();

    void load();
    void save() const;

    void setGuestInputPermissions(uint32_t userID, bool gamepad, bool keyboard, bool mouse);
    bool tryGetGuestInputPermissions(uint32_t userID, GuestPermissionState& state) const;

private:
    GuestStateStore() = default;

    std::string getFilePath() const;

    std::vector<GuestPermissionState> _guestInputPermissions;
};
