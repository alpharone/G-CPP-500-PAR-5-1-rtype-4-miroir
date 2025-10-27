/*
** EPITECH PROJECT, 2025
** repo
** File description:
** SpriteManager
*/

#pragma once
#include <mutex>
#include <raylib.h>
#include <string>
#include <unordered_map>

class SpriteManager {
public:
  SpriteManager() = default;
  ~SpriteManager();
  Texture2D load(const std::string &path);
  void unloadAll();
  Texture2D getTexture(const std::string &name);

private:
  std::unordered_map<std::string, Texture2D> _cache;
  std::mutex _mtx;
};
