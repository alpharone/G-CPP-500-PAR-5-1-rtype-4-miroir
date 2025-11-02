/*
** EPITECH PROJECT, 2025
** repo
** File description:
** Drawable
*/

#pragma once

#include <cstdint>
#include <raylib.h>
#include <string>
#include <unordered_map>

namespace Component {

struct drawable_t {
  std::string renderType = "sprite";
  uint32_t resourceId = 0;
  int layer = 0;
  float scaleX = 1.0f;
  float scaleY = 1.0f;
  std::unordered_map<std::string, std::string> meta;

  drawable_t(const std::string &type, uint32_t id = 0, int l = 0,
             float sx = 1.0f, float sy = 1.0f)
      : renderType(type), resourceId(id), layer(l), scaleX(sx), scaleY(sy) {}
};

} // namespace Component