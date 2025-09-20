/*
** EPITECH PROJECT, 2025
** bootstrap2
** File description:
** main
*/

#include <iostream>
#include "Entity.hpp"
#include "SparseArray.hpp"
#include "Components.hpp"
#include "Registry.hpp"
#include "IndexedZipper.hpp"
#include "System.hpp"

int main()
{
    Ecs::SparseArray<Components::position_t> positions;
    Ecs::SparseArray<Components::velocity_t> velocities;

    positions.emplace_at(0, 1.f, 1.f);
    velocities.emplace_at(0, 0.5f, 0.25f);

    velocities.emplace_at(1, 4.f, 5.f);
    positions.emplace_at(2, 5.f, 5.f);
    velocities.emplace_at(2, 1.f, 1.f);

    auto zipper = Ecs::makeIndexedZipper(positions, velocities);

    for (auto&& [entity, pos, vel] : zipper) {
        if (pos && vel) {
            std::cout << "Entity " << entity
                      << " pos=(" << pos->x << "," << pos->y << ")"
                      << " vel=(" << vel->vx << "," << vel->vy << ")\n";
        }
    }
}
