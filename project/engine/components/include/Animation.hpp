/*
** EPITECH PROJECT, 2025
** Components
** File description:
** Animation
*/

#pragma once

#include <string>

namespace Component {

    struct animation_t {
        int frameW = 0;
        int frameH = 0;

        int frameCount = 1;

        int startX = 0;
        int startY = 0;

        int fps = 12;

        bool loop = true;

        int currentFrame = 0;
        double elapsed = 0.0;

        std::string name;

        animation_t() = default;
        animation_t(int fw, int fh, int count, int fps_ = 12, bool loop_ = true, int sx = 0, int sy = 0, std::string nm = "")
            : frameW(fw), frameH(fh), frameCount(count), startX(sx), startY(sy), fps(fps_), loop(loop_), name(std::move(nm)) {}
    };

}
