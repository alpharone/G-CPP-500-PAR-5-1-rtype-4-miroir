/*
** EPITECH PROJECT, 2025
** repo
** File description:
** WIndowSize
*/

#pragma once

namespace Component {

    struct windowSize_t {
        unsigned int w;
        unsigned int h;

        windowSize_t() = default;
        windowSize_t(unsigned int _w, unsigned int _h) noexcept :
        w(_w), h(_h){}
    };

}
