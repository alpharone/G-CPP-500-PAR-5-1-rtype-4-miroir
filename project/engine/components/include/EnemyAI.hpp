/*
** EPITECH PROJECT, 2025
** repo
** File description:
** EnemyAI
*/

#pragma once

namespace Component {

struct enemy_ai_t {
  int pattern;
  float cooldown;

  enemy_ai_t() = default;
  enemy_ai_t(int _pattern, float _cooldown) noexcept
      : pattern(_pattern), cooldown(_cooldown) {}
};

} // namespace Component
