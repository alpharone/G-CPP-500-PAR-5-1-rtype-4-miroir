/*
** EPITECH PROJECT, 2025
** Components
** File description:
** Health
*/

#pragma once

namespace Component {

struct health_t {
  int hp = 100;
  int maxHp = 100;

  health_t() = default;
  health_t(int _hp) noexcept : hp(_hp), maxHp(_hp) {}
  health_t(int _hp, int _maxHp) noexcept : hp(_hp), maxHp(_maxHp) {}
};

} // namespace Component
