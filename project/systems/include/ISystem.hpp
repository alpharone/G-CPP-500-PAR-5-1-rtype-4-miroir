/*
** EPITECH PROJECT, 2025
** Systems
** File description:
** ISystem.hpp
*/

#pragma once

#include "Registry.hpp"

class ISystem {
    public:
        virtual ~ISystem() = default;
        virtual void update(Ecs::Registry& r, float dt) = 0;
};
