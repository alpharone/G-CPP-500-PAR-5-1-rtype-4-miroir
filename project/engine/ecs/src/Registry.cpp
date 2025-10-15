/*
** EPITECH PROJECT, 2025
** bootstrap2
** File description:
** Registry
*/

#include "Registry.hpp"

Ecs::Entity Ecs::Registry::spawnEntity()
{
    if (!_free_ids.empty()) {
        auto id = _free_ids.back();
        _free_ids.pop_back();
        return Entity(id);
    }
    return Entity(_next_id++);
}

void Ecs::Registry::killEntity(Entity const& Entity)
{
    for (auto const& cb : _erase_callbacks) {
        cb(*this, Entity);
    }
    _free_ids.push_back(static_cast<size_t>(Entity));
}

void Ecs::Registry::runSystems()
{
    for (auto& sys : _systems) {
        sys(*this);
    }
}
