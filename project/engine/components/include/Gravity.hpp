/*
** EPITECH PROJECT, 2025
** repo
** File description:
** gravity
*/

#pragma once

namespace Component {
    
    struct gravity_t {
    public:
        explicit gravity_t(float _strength = 9.81f) noexcept : strength(_strength) {}

        float strength = 9.81f;
    };
}