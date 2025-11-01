/*
** EPITECH PROJECT, 2025
** Components
** File description:
** collision
*/

#pragma once

namespace Component {

enum class CollisionType {
    None = 0,
    Player,
    Enemy,
    Projectile,
    Wall
};

struct collision_type_t {
    CollisionType type = CollisionType::None;
};

struct collision_t {
    float width = 1.0f;
    float height = 1.0f;
    float depth = 0.0f;

    bool isSolid = true;
    bool isTrigger = false;

    collision_t() = default;
    collision_t(float w, float h, float d = 0.0f,
                bool solid = true, bool trigger = false)
        : width(w), height(h), depth(d), isSolid(solid), isTrigger(trigger) {}
};

}
