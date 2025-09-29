/*
** EPITECH PROJECT, 2025
** Client
** File description:
** ClientApp.hpp
*/

#pragma once

#include <chrono>
#include "Registry.hpp"
#include "ResourceManager.hpp"
#include "RenderLoop.hpp"

class ClientApp {
    public:
        ClientApp();
        ~ClientApp();

        bool init();
        void mainLoop();
        void shutdown();

    private:
        Ecs::Registry _registry;
        ResourceManager _res;
        RenderLoop _render;
        std::chrono::steady_clock::time_point _lastTime;
        Ecs::Entity _playerEntity;

        float computeDeltaTime();
        void handleEvents();
        std::pair<float, float> getMovementInput() const;
        void movePlayer(float dx, float dy, float dt);
};
