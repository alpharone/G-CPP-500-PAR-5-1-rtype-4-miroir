/*
** EPITECH PROJECT, 2025
** repo
** File description:
** SystemManager
*/

#include "SystemManager.hpp"
#include "Logger.hpp"

void System::SystemManager::registerSystem(std::shared_ptr<ISystem> system) {
  _systems.push_back(system);
}

void System::SystemManager::registerSystem(const std::string &name,
                                           const std::string &path,
                                           const std::string &entrypoint) {
  _catalog.registerSystem(name, path, entrypoint);
}

void System::SystemManager::addSystem(const std::string &name,
                                      std::any params) {
  auto sys = _catalog.loadSystem(name, params);
  _systems.push_back(sys);
}

void System::SystemManager::updateAll(Ecs::Registry &registry, double dt) {
  for (auto &sys : _systems)
    sys->update(registry, dt);
}

void System::SystemManager::initAll(Ecs::Registry &registry) {
  for (auto &sys : _systems)
    sys->init(registry);
}

void System::SystemManager::shutdownAll() {
  for (auto &sys : _systems) {
    try {
      sys->shutdown();
    } catch (const std::exception &e) {
      Logger::error(std::string("[SystemManager] Shutdown failed: ") +
                    e.what());
    }
  }
}

void System::SystemManager::clearAll() {
  for (size_t i = _systems.size(); i-- > 0;) {
    try {
      _systems[i].reset();
    } catch (const std::exception &e) {
      Logger::error(std::string("[SystemManager] Clear failed: ") + e.what());
    }
  }
  _systems.clear();
}
