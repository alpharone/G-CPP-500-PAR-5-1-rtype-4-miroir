/*
** EPITECH PROJECT, 2025
** repo
** File description:
** main
*/

#include "SystemManager.hpp"
#include "Logger.hpp"
#include "Registry.hpp"
#include <chrono>
#include <thread>

int main() {
    Logger::init("logs/server.log");
    Ecs::Registry registry;

    SystemCatalog catalog;
    SystemManager manager;

    catalog.registerSystem("server_net", "./plugins/systems/libServerNetworkSystem.so", "createServerNetworkSystem");
    auto serverNet = catalog.loadSystem("server_net", (unsigned short)4242);
    manager.registerSystem(serverNet);

    Logger::info("[Server] Running...");
    const float dt = 1.f/60.f;
    while (true) {
        manager.updateAll(registry, dt);
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }

    Logger::shutdown();
    return 0;
}
