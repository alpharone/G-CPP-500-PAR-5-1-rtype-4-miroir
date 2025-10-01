/*
** EPITECH PROJECT, 2025
** repo
** File description:
** Velocity
*/

#pragma once

namespace Component {

    struct velocity_t {
        float vx;
        float vy;

        velocity_t() = default;
        velocity_t(float _vx, float _vy) noexcept :
        vx(_vx), vy(_vy){}
    };

}