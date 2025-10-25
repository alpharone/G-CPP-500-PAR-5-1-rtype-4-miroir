/*
** EPITECH PROJECT, 2025
** repo
** File description:
** SystemCatalog
*/

#include "SystemCatalog.hpp"

void SystemCatalog::registerSystem(const std::string &name,
                                   const std::string &path,
                                   const std::string &entrypoint) {
  info[name] = {path, entrypoint};
}

std::shared_ptr<ISystem> SystemCatalog::loadSystem(const std::string &name,
                                                   std::any params) {
  auto it = info.find(name);
  if (it == info.end()) {
    throw std::runtime_error("SystemCatalog: system not registered: " + name);
  }
  if (loaders.find(name) == loaders.end()) {
    loaders[name] = std::make_unique<DLLoader<ISystem>>(it->second.first,
                                                        it->second.second);
  }
  return loaders[name]->createInstance(params);
}
