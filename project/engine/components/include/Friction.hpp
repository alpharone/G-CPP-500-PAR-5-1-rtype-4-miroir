/*
** EPITECH PROJECT, 2025
** repo
** File description:
** friction
*/

#pragma once

namespace Component {

    struct friction_t {
        int friction;

        explicit friction_t(int _friction) noexcept :
        friction(_friction) {}
    };

}