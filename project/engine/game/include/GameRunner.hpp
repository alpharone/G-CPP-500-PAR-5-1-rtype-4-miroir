/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** GameRunner
*/

#pragma once

#include "NetworkContext.hpp"
#include "Registry.hpp"
#include "SystemCatalog.hpp"
#include "SystemManager.hpp"
#include <any>
#include <memory>
#include <nlohmann/json.hpp>
#include <string>
#include <unordered_map>

class GameRunner {
public:
  GameRunner(const std::string &configFile);
  ~GameRunner() = default;

  void run();

private:
  void loadConfig(const std::string &configFile);
  void initSystems();
  void applyConfig();

  Ecs::Registry registry;
  System::SystemManager manager;
  System::SystemCatalog catalog;

  std::unordered_map<std::string, std::any> globals;

  struct SystemConfig {
    std::string name;
    std::string lib;
    std::string factory;
    std::vector<std::string> params;
  };
  std::vector<SystemConfig> systemsToLoad;

  std::unordered_map<std::string, std::any> configData;
};
