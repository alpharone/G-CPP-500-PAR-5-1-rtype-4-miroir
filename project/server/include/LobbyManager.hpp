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
    LobbyManager() : _nextClientId(1) {}

    uint32_t registerClient(const std::string& key) {
        auto it = _byKey.find(key);
        if (it != _byKey.end()) return it->second.clientId;
        uint32_t id = _nextClientId++;
        PlayerInfo pi{id, key, false};
        _byKey[key] = pi;
        _byId[id] = pi;
        return id;
    }

    void setReady(uint32_t clientId, bool ready) {
        auto it = _byId.find(clientId);
        if (it != _byId.end()) {
            it->second.ready = ready;
            _byKey[it->second.addrKey].ready = ready;
        }
    }

    bool allReady() const {
        if (_byId.empty()) return false;
        for (auto &p : _byId) if (!p.second.ready) return false;
        return true;
    }

private:
    uint32_t _nextClientId;
    std::unordered_map<std::string, PlayerInfo> _byKey;
    std::unordered_map<uint32_t, PlayerInfo> _byId;
};