/*
** EPITECH PROJECT, 2025
** repo
** File description:
** SystemManager
*/

#include "SystemManager.hpp"
#include "Logger.hpp"

void SystemManager::registerSystem(std::shared_ptr<ISystem> system)
{
    _systems.push_back(system);
}

void SystemManager::registerSystem(const std::string& name, const std::string& path, const std::string& entrypoint)
{
    _catalog.registerSystem(name, path, entrypoint);
}

void SystemManager::addSystem(const std::string& name, std::any params)
{
    auto sys = _catalog.loadSystem(name, params);
    _systems.push_back(sys);
}

void SystemManager::updateAll(Ecs::Registry& registry, float dt)
{
    for (auto& sys : _systems)
        sys->update(registry, dt);
}
