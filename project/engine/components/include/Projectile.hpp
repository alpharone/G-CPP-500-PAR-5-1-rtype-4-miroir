/*
** EPITECH PROJECT, 2025
** repo
** File description:
** Projectile
*/

#pragma once

namespace Component {

    struct projectile_t {
        float dmg;
        int ownerId;

        projectile_t() = default;
        projectile_t(float _dmg, int _ownerId) noexcept :
        dmg(_dmg), ownerId(_ownerId){}
    };

}