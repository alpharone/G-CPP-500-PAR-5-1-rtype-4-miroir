/*
** EPITECH PROJECT, 2025
** repo
** File description:
** DLLoader
*/

#pragma once

#include <string>
#include <dlfcn.h>
#include <iostream>
#include <stdexcept>
#include <any>

template <typename T>
class DLLoader {
    public:
        using Func = std::shared_ptr<T>(*)(std::any);
        DLLoader(const std::string& filePath, const std::string& entrypoint) : handle(nullptr), func(nullptr) {
            handle = dlopen(filePath.c_str(), RTLD_LAZY);
            if (!handle) {
                throw std::runtime_error("Failed to open shared library: " + std::string(dlerror()));
            }

            func = reinterpret_cast<std::shared_ptr<T>(*)(std::any)>(dlsym(handle, entrypoint.c_str()));
            if (!func) {
                dlclose(handle);
                throw std::runtime_error("Failed to get entrypoint function: " + std::string(dlerror()));
            }
        }

        ~DLLoader() {
            if (handle) {
                dlclose(handle);
            }
        }

        std::shared_ptr<T> createInstance(std::any params) {
            return func(params);
        }
    
    private:
        void* handle;
        Func func;
};