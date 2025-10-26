/*
** EPITECH PROJECT, 2025
** repo
** File description:
** DLLoader
*/

#pragma once

#include <any>
#include <dlfcn.h>
#include <iostream>
#include <stdexcept>
#include <string>

template <typename T> class DLLoader {
public:
  using Func = std::shared_ptr<T> (*)(std::any);
  DLLoader(const std::string &filePath, const std::string &entrypoint)
      : _handle(nullptr), _func(nullptr) {
    if (filePath.empty()) {
      throw std::invalid_argument("DLLoader: filePath is empty");
    }
    if (entrypoint.empty()) {
      throw std::invalid_argument("DLLoader: entrypoint is empty");
    }

    _handle = dlopen(filePath.c_str(), RTLD_NOW | RTLD_LOCAL);
    if (!_handle) {
      throw std::runtime_error("Failed to open shared library: " +
                               std::string(dlerror()));
    }

    _func = reinterpret_cast<std::shared_ptr<T> (*)(std::any)>(
        dlsym(_handle, entrypoint.c_str()));
    if (!_func) {
      dlclose(_handle);
      throw std::runtime_error("Failed to get entrypoint function: " +
                               std::string(dlerror()));
    }
  }

  ~DLLoader() {
    if (_handle) {
      dlclose(_handle);
      _handle = nullptr;
    }
  }

  std::shared_ptr<T> createInstance(std::any params) { return _func(params); }

private:
  void *_handle;
  Func _func;
};