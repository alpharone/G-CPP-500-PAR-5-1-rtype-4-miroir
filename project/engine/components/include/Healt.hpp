/*
** EPITECH PROJECT, 2025
** repo
** File description:
** Healt
*/

#pragma once

namespace Component {

    struct healt_t {
        int healt;

        explicit healt_t(int _healt=100) noexcept :
        healt(_healt) {}
    };

}