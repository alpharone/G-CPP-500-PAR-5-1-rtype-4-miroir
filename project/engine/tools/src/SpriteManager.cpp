/*
** EPITECH PROJECT, 2025
** repo
** File description:
** SpriteManager
*/

#include "SpriteManager.hpp"

SpriteManager::~SpriteManager()
{
    unloadAll();
}

Texture2D SpriteManager::load(const std::string& path)
{
    if (path.empty())
        return Texture2D{0};

    std::lock_guard<std::mutex> lock(_mtx);
    auto it = _cache.find(path);
    if (it != _cache.end())
        return it->second;

    Texture2D tex = LoadTexture(path.c_str());
    _cache.emplace(path, tex);
    return _cache[path];
}

void SpriteManager::unloadAll()
{
    std::lock_guard<std::mutex> lock(_mtx);

    for (auto &kv : _cache) {
        if (kv.second.id != 0) {
            UnloadTexture(kv.second);
            kv.second.id = 0;
        }
    }
    _cache.clear();
}
