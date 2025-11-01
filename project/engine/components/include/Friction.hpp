/*
** EPITECH PROJECT, 2025
** repo
** File description:
** friction
*/

#pragma once

namespace Component {

    struct friction_t {
    public: 
        explicit friction_t(float _drag = 0.1f) noexcept : drag(_drag) {}
    
        float drag = 0.1f;
    };

}