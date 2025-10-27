/*
** EPITECH PROJECT, 2025
** repo
** File description:
** friction
*/

#pragma once
#include <string>

namespace Component {

    struct sprite_t {
        std::string sprite;

        explicit sprite_t(std::string _sprite) noexcept :
        sprite(_sprite) {}
    };

}