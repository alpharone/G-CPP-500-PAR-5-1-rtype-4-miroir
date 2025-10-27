/*
** EPITECH PROJECT, 2025
** repo
** File description:
** DlLoader
*/

#pragma once

#include <any>
#include <dlfcn.h>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>

template <typename T> class DlLoader {
public:
  using Func = std::shared_ptr<T> (*)(std::any);
  DlLoader(const std::string &filePath, const std::string &entrypoint)
      : _handle(nullptr), _func(nullptr) {
    if (filePath.empty()) {
      throw std::invalid_argument("DlLoader: filePath is empty");
    }
    if (entrypoint.empty()) {
      throw std::invalid_argument("DlLoader: entrypoint is empty");
    }
    _handle = dlopen(filePath.c_str(), RTLD_NOW | RTLD_GLOBAL);
    if (!_handle) {
      const char *err = dlerror();
      throw std::runtime_error("Failed to open shared library: " +
                               (err ? std::string(err) : "unknown error"));
    }
    _func = reinterpret_cast<std::shared_ptr<T> (*)(std::any)>(
        dlsym(_handle, entrypoint.c_str()));
    if (!_func) {
      dlclose(_handle);
      const char *err = dlerror();
      throw std::runtime_error("Failed to get entrypoint function: " +
                               (err ? std::string(err) : "unknown error"));
    }
  }

  ~DlLoader() {
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
