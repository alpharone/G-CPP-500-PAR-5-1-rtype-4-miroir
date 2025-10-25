/*
** EPITECH PROJECT, 2025
** repo
** File description:
** Position
*/

#pragma once

namespace Component {

struct position_t {
  float x;
  float y;

  position_t() = default;
  position_t(float _x, float _y) noexcept : x(_x), y(_y) {}
};

} // namespace Component