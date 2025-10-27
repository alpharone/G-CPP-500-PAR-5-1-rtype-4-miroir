/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** GameRunner
*/

#include "GameRunner.hpp"
#include "Logger.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

GameRunner::GameRunner(const std::string &configFile) {
  loadConfig(configFile);
  auto netCtx = std::make_shared<Network::network_context_t>();
  netCtx->registry = &registry;
  globals["netCtx"] = netCtx;
  initSystems();
  applyConfig();
}

void GameRunner::loadConfig(const std::string &configFile) {
  std::ifstream file(configFile);
  if (!file.is_open()) {
    std::cerr << "Cannot open config file: " << configFile << std::endl;
    return;
  }

  nlohmann::json cfg;
  try {
    file >> cfg;
  } catch (const nlohmann::json::parse_error &e) {
    std::cerr << "JSON parse error: " << e.what() << std::endl;
    return;
  }

  if (cfg.contains("globals")) {
    for (auto &[key, value] : cfg["globals"].items()) {
      if (value.is_string()) {
        globals[key] = value.get<std::string>();
      } else if (value.is_number_integer()) {
        globals[key] = value.get<int>();
      }
    }
  }

  if (cfg.contains("systems")) {
    for (const auto &sys : cfg["systems"]) {
      SystemConfig sc;
      sc.name = sys["name"];
      sc.lib = sys["lib"];
      sc.factory = sys["factory"];
      for (const auto &param : sys["params"]) {
        sc.params.push_back(param);
      }
      systemsToLoad.push_back(sc);
    }
  }
}

void GameRunner::initSystems() {
  for (const auto &sc : systemsToLoad) {
    catalog.registerSystem(sc.name, sc.lib, sc.factory);

    std::vector<std::any> params;
    for (const auto &paramKey : sc.params) {
      if (globals.count(paramKey)) {
        params.push_back(globals[paramKey]);
      } else {
        std::cerr << "Warning: param " << paramKey << " not found in globals"
                  << std::endl;
      }
    }
    std::any paramAny = params;

    auto sys = catalog.loadSystem(sc.name, paramAny);
    manager.registerSystem(sys);
  }

  manager.initAll(registry);
}

void GameRunner::applyConfig() {}

void GameRunner::run() {
  const float dt = 1.f / 60.f;
  bool isClient = false;
  for (const auto &sc : systemsToLoad) {
    if (sc.name == "render") {
      isClient = true;
      break;
    }
  }
  while (true) {
    manager.updateAll(registry, dt);
    if (globals.count("netCtx")) {
      auto ctx = std::any_cast<std::shared_ptr<Network::network_context_t>>(
          globals["netCtx"]);
      if (ctx->quitRequested)
        break;
    }
    if (!isClient) {
      std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }
  }
  Logger::info("[GameRunner] Shutting down systems...");
  manager.shutdownAll();
  manager.clearAll();
  std::this_thread::sleep_for(std::chrono::milliseconds(10));
  Logger::shutdown();
}
