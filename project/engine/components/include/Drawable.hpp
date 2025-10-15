/*
** EPITECH PROJECT, 2025
** repo
** File description:
** Drawable
*/

#pragma once

#include <cstdint>
#include <string>
#include <raylib.h>

namespace Component {

    struct drawable_t {
        std::string texturePath;
        Color color = WHITE;
        bool isPlayer = false;
        int z = 0;
        Texture2D texture{ 0 };
        bool loaded = false;
        
        drawable_t() = default;
        drawable_t(std::string _texturePath, Color _color, bool _isPlayer, int _z, Texture2D _texture, bool _loaded) noexcept :
        texturePath(_texturePath), color(_color), isPlayer(_isPlayer), z(_z), texture(_texture), loaded(_loaded) {}
    };

}