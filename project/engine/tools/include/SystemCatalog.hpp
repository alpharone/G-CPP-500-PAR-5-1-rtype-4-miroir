/*
** EPITECH PROJECT, 2025
** repo
** File description:
** CatalogSystem
*/

#pragma once

#include <any>
#include <iostream>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include "DLLoader.hpp"
#include "ISystem.hpp"

class SystemCatalog {
public:
  SystemCatalog() = default;
  void registerSystem(const std::string &name, const std::string &path,
                      const std::string &entrypoint);

  std::shared_ptr<ISystem> loadSystem(const std::string &name,
                                      std::any params = {});

private:
  std::map<std::string, std::pair<std::string, std::string>> info;
  std::map<std::string, std::unique_ptr<DLLoader<ISystem>>> loaders;
};
