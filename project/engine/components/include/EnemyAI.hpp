/*
** EPITECH PROJECT, 2025
** repo
** File description:
** EnemyAI
*/

#pragma once

namespace Component {

struct enemyAI_t {
  int pattern;
  float cooldown;

  enemyAI_t() = default;
  enemyAI_t(int _pattern, float _cooldown) noexcept
      : pattern(_pattern), cooldown(_cooldown) {}
};

} // namespace Component