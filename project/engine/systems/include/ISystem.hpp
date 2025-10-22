/*
** EPITECH PROJECT, 2025
** Systems
** File description:
** ISystem.hpp
*/

#pragma once

#include "Registry.hpp"

namespace System {

    class ISystem {
        public:
            virtual ~ISystem() = default;
            virtual void init(Ecs::Registry& registry) = 0;
            virtual void update(Ecs::Registry& registry, double dt) = 0;
            virtual void shutdown() = 0;
    };

}
