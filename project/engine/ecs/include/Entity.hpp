/*
** EPITECH PROJECT, 2025
** bootstrap2
** File description:
** Entity
*/

#pragma once

#include <cstddef>

namespace Ecs {

class Entity {
public:
  explicit Entity(size_t id = 0) noexcept;

  operator size_t() const noexcept;
  bool operator==(Entity const &other) const noexcept;
  bool operator!=(Entity const &other) const noexcept;

private:
  size_t _id;
};

} // namespace Ecs