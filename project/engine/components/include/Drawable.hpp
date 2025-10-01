/*
** EPITECH PROJECT, 2025
** repo
** File description:
** Drawable
*/

#pragma once

#include <cstdint>
#include <string>

namespace Component {

    struct drawable_t {
        uint16_t w;
        uint16_t h;
        uint32_t color;
        std::string spriteId;

        drawable_t() = default;
        drawable_t(uint16_t _w, uint16_t _h, uint32_t _color, std::string _spriteId) noexcept :
        w(_w), h(_h), color(_color), spriteId(_spriteId){}
    };

}