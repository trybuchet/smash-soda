#pragma once
#include <string>
#include <sstream>

struct IPRangeStruct {
    uint32_t start;
    uint32_t end;

    IPRangeStruct(const std::string& cidr) {
        size_t pos = cidr.find('/');
        std::string ip = cidr.substr(0, pos);
        int prefix = std::stoi(cidr.substr(pos + 1));

        uint32_t ip_num = ipToUint(ip);
        uint32_t mask = (prefix == 0) ? 0 : (~0U << (32 - prefix));

        start = ip_num & mask;
        end = start | ~mask;
    }

    static uint32_t ipToUint(const std::string& ip) {
        std::istringstream iss(ip);
        std::string token;
        uint32_t result = 0;

        for (int i = 0; i < 4; ++i) {
            std::getline(iss, token, '.');
            result = (result << 8) | std::stoi(token);
        }

        return result;
    }
};