/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** GameRunner
*/

#include "GameRunner.hpp"
#include "ClientConfig.hpp"
#include "Logger.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <sstream>
#include <thread>

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

  fullConfig = cfg;

  if (cfg.contains("globals")) {
    for (auto &[key, value] : cfg["globals"].items()) {
      if (value.is_string()) {
        globals[key] = value.get<std::string>();
      } else if (value.is_number_integer()) {
        globals[key] = value.get<int>();
      } else if (value.is_number_float()) {
        globals[key] = value.get<float>();
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

    std::any paramAny;
    if (sc.factory == "createClientNetworkSystem") {
      System::client_network_factory_params_t factoryParams;
      if (sc.params.size() >= 10) {
        factoryParams.host =
            std::any_cast<std::string>(resolveParameter(sc.params[0]));
        factoryParams.port = std::any_cast<int>(resolveParameter(sc.params[1]));
        factoryParams.ctx =
            std::any_cast<std::shared_ptr<Network::network_context_t>>(
                resolveParameter(sc.params[2]));
        factoryParams.sprite =
            std::any_cast<std::string>(resolveParameter(sc.params[3]));
        factoryParams.frameW =
            std::any_cast<int>(resolveParameter(sc.params[4]));
        factoryParams.frameH =
            std::any_cast<int>(resolveParameter(sc.params[5]));
        factoryParams.frameCount =
            std::any_cast<int>(resolveParameter(sc.params[6]));
        factoryParams.frameTime =
            std::any_cast<float>(resolveParameter(sc.params[7]));
        factoryParams.frameX =
            std::any_cast<int>(resolveParameter(sc.params[8]));
        factoryParams.frameY =
            std::any_cast<int>(resolveParameter(sc.params[9]));
      }
      paramAny = factoryParams;
    } else {
      std::vector<std::any> params;
      for (const auto &paramKey : sc.params) {
        try {
          params.push_back(resolveParameter(paramKey));
        } catch (const std::exception &e) {
          std::cerr << "Warning: param " << paramKey
                    << " not found: " << e.what() << std::endl;
        }
      }
      paramAny = params;
    }

    auto sys = catalog.loadSystem(sc.name, paramAny);
    manager.registerSystem(sys);
  }

  manager.initAll(registry);
}

std::any GameRunner::resolveParameter(const std::string &paramKey) {
  if (globals.count(paramKey)) {
    return globals[paramKey];
  }

  if (paramKey == "config") {
    return fullConfig;
  }

  nlohmann::json current = fullConfig;
  std::stringstream ss(paramKey);
  std::string part;
  while (std::getline(ss, part, '.')) {
    if (!current.contains(part)) {
      break;
    }
    current = current[part];
  }

  if (ss.eof()) {
    if (current.is_string()) {
      return current.get<std::string>();
    } else if (current.is_number_integer()) {
      return current.get<int>();
    } else if (current.is_number_float()) {
      return current.get<float>();
    }
  }

  throw std::runtime_error("Parameter not found: " + paramKey);
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
