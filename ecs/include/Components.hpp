/*
** EPITECH PROJECT, 2025
** bootstrap2
** File description:
** Components
*/

#pragma once
#include <SFML/Graphics/Color.hpp>

namespace Components {

    struct position_t {
        float x;
        float y;
        
        position_t() = default;
        position_t(float X, float Y) noexcept : x(X), y(Y) {}
    };

    struct velocity_t {
        float vx;
        float vy;

        velocity_t() = default;
        velocity_t(float VX, float VY) noexcept : vx(VX), vy(VY) {}
    };

    struct drawable_t {
        float w;
        float h;
        sf::Color color;

        drawable_t() = default;
        drawable_t(float W, float H, sf::Color C) noexcept : w(W), h(H), color(C) {}
    };

    struct controllable_t {
    };

}