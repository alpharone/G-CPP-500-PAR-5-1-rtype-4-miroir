/*
** EPITECH PROJECT, 2025
** Client
** File description:
** main.cpp
*/

#include <raylib.h>
#include <chrono>
#include <thread>
#include <asio.hpp>
#include "SystemManager.hpp"
#include "Logger.hpp"
#include "Registry.hpp"
#include "NetworkContext.hpp"

int main()
{
    Logger::init("logs/client.log");
    Ecs::Registry registry;
    SystemManager manager;
    SystemCatalog catalog;

    auto netCtx = std::make_shared<NetworkContext>();

    catalog.registerSystem("client_net", "./plugins/systems/libClientNetworkSystem.so", "createClientNetworkSystem");
    catalog.registerSystem("render", "./plugins/systems/libRenderSystem.so", "createRenderSystem");
    catalog.registerSystem("input", "./plugins/systems/libInputSystem.so", "createInputSystem");

    auto render = catalog.loadSystem("render", std::any(std::make_tuple(800, 600, std::string("R-Type Client"), netCtx)));
    auto clientNet = catalog.loadSystem("client_net", std::any(std::make_tuple(std::string("127.0.0.1"), (unsigned short)4242, netCtx)));
    auto input = catalog.loadSystem("input", std::any(netCtx));

    manager.registerSystem(render);
    manager.registerSystem(input);
    manager.registerSystem(clientNet);

    const float dt = 1.f / 60.f;
    while (!WindowShouldClose()) {
        manager.updateAll(registry, dt);
    }
    Logger::info("[Client] Shutting down systems...");
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    Logger::shutdown();
    return 0;
}
