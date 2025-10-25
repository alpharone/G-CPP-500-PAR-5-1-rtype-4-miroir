/*
** EPITECH PROJECT, 2025
** repo
** File description:
** SystemManager
*/

#pragma once

#include <memory>
#include <vector>

#include "ISystem.hpp"
#include "Registry.hpp"
#include "SystemCatalog.hpp"

namespace System {

class SystemManager {
public:
  SystemManager() = default;
  ~SystemManager() = default;

  void registerSystem(std::shared_ptr<ISystem> system);
  void registerSystem(const std::string &name, const std::string &path,
                      const std::string &entrypoint);
  void addSystem(const std::string &name, std::any params = {});
  void initAll(Ecs::Registry &registry);
  void updateAll(Ecs::Registry &registry, double dt);
  void shutdownAll();
  void clearAll();

private:
  std::vector<std::shared_ptr<ISystem>> _systems;
  SystemCatalog _catalog;
};

} // namespace System
