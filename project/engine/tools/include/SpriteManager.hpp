/*
** EPITECH PROJECT, 2025
** repo
** File description:
** SpriteManager
*/

#pragma once
#include <string>
#include <unordered_map>
#include <mutex>
#include <raylib.h>

class SpriteManager {
    public:
        SpriteManager() = default;
        ~SpriteManager();

        Texture2D load(const std::string& path);
        void unloadAll();

    private:
        std::unordered_map<std::string, Texture2D> _cache;
        std::mutex _mtx;
};