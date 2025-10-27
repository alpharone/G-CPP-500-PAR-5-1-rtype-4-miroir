/*
** EPITECH PROJECT, 2025
** repo
** File description:
** EnemyAI
*/

#pragma once
#include <string>

namespace Component {

    struct sound_t {
        std::string sound;
        bool play;
        float volume;

        sound_t() = default;
        sound_t(std::string _sound, bool _play=false, float _volume=1.f) noexcept :
        sound(_sound), play(_play), volume(_volume){}
    };

}