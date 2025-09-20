/*
** EPITECH PROJECT, 2025
** bootstrap2
** File description:
** System
*/

#include "System.hpp"
#include "Components.hpp"

void System::positionSystem(Ecs::Registry& reg)
{
    auto& positions = reg.getComponents<Components::position_t>();
    auto& velocities = reg.getComponents<Components::velocity_t>();

    for (size_t i = 0; i < positions.size() && i < velocities.size(); i += 1) {
        auto& pos = positions[i];
        auto& vel = velocities[i];
        if (pos && vel) {
            pos->x += vel->vx;
            pos->y += vel->vy;
        }
    }
}
