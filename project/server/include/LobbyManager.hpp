/*
** EPITECH PROJECT, 2025
** repo
** File description:
** LobbyManager
*/

#pragma once
#include <map>
#include <vector>
#include "Connection.hpp"

#include <unordered_map>
#include <vector>
#include <cstdint>

struct PlayerInfo {
    uint32_t clientId;
    std::string addrKey;
    bool ready = false;
};

class LobbyManager {
    public:
        LobbyManager();

        uint32_t registerClient(const std::string& key);
        void setReady(uint32_t clientId, bool ready);
        bool allReady() const;

    private:
        uint32_t _nextClientId;
        std::unordered_map<std::string, PlayerInfo> _byKey;
        std::unordered_map<uint32_t, PlayerInfo> _byId;
};
