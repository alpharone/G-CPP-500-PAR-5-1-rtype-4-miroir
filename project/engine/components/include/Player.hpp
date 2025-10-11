/*
** EPITECH PROJECT, 2025
** repo
** File description:
** Player
*/

#pragma once

namespace Component {

    struct player_t {
        unsigned int id;

        player_t() = default;
        player_t(unsigned int _id) noexcept : id(_id){};
    };

}