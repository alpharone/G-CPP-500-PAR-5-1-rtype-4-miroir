/*
** EPITECH PROJECT, 2025
** repo
** File description:
** Healt
*/

#pragma once

namespace Component {

    struct gravity_t {
        int gravity;

        explicit gravity_t(int _gravity=0) noexcept :
        gravity(_gravity) {}
    };

}