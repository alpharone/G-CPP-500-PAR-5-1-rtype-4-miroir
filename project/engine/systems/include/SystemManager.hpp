/*
** EPITECH PROJECT, 2025
** repo
** File description:
** SystemManager
*/

#pragma once

#include <vector>
#include <memory>
#include "ISystem.hpp"
#include "Registry.hpp"
#include "SystemCatalog.hpp"

class SystemManager {
    public:
        SystemManager() = default;
        ~SystemManager() = default;
    
        void registerSystem(std::shared_ptr<ISystem> system);
        void registerSystem(const std::string& name, const std::string& path, const std::string& entrypoint);    
        void addSystem(const std::string& name, std::any params = {});
        void updateAll(Ecs::Registry& registry, float dt);
    
    private:
        std::vector<std::shared_ptr<ISystem>> _systems;
        SystemCatalog _catalog;
};