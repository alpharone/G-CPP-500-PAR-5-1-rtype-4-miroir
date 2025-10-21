/*
** EPITECH PROJECT, 2025
** repo
** File description:
** LobbyManager
*/

#include "LobbyManager.hpp"

LobbyManager::LobbyManager() : _nextClientId(1)
{}

uint32_t LobbyManager::registerClient(const std::string& key)
{
    auto it = _byKey.find(key);
    if (it != _byKey.end())
        return it->second.clientId;
    uint32_t id = _nextClientId++;
    PlayerInfo pi{id, key, false};
    _byKey[key] = pi;
    _byId[id] = pi;
    return id;
}

void LobbyManager::setReady(uint32_t clientId, bool ready)
{
    auto it = _byId.find(clientId);

    if (it != _byId.end()) {
        it->second.ready = ready;
        _byKey[it->second.addrKey].ready = ready;
    }
}

bool LobbyManager::allReady() const
{
    if (_byId.empty())
        return false;
    for (auto &p : _byId) {
        if (!p.second.ready)
            return false;
    }
    return true;
}
