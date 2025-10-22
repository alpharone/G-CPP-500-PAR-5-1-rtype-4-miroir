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
#include <filesystem>
#include "SystemManager.hpp"
#include "Logger.hpp"
#include "Registry.hpp"
#include "NetworkContext.hpp"
#include "Position.hpp"
#include "Animation.hpp"
#include "Drawable.hpp"

int main()
{
    Logger::init("logs/client.log");
    Ecs::Registry registry;
    System::SystemManager manager;
    System::SystemCatalog catalog;

    auto netCtx = std::make_shared<Network::network_context_t>();

    netCtx->registry = &registry;
    catalog.registerSystem("render", "./plugins/systems/libRenderSystem.so", "createRenderSystem");
    catalog.registerSystem("input", "./plugins/systems/libInputSystem.so", "createInputSystem");
    catalog.registerSystem("client", "./plugins/systems/libClientNetworkSystem.so", "createClientNetworkSystem");

    auto render = catalog.loadSystem("render", std::any(std::make_tuple(1200, 1000, std::string("R-Type Client"), netCtx)));
    auto clientnet = catalog.loadSystem("client", std::any(std::make_tuple(std::string("127.0.0.1"), (unsigned short)4242, netCtx)));
    auto input = catalog.loadSystem("input", std::any(netCtx));

    manager.registerSystem(clientnet);
    manager.registerSystem(render);
    manager.registerSystem(input);

    manager.initAll(registry);


    Logger::info("[Client] Creating a test entity for rendering debug");

    Logger::info(std::string("Asset exists: ") + (std::filesystem::exists("assets/sprites/r-typesheet42.gif") ? "yes" : "no"));


    const float dt = 1.f / 60.f;
    while (!WindowShouldClose()) {
        manager.updateAll(registry, dt);
        if (netCtx->quitRequested)
            break;
    }
    Logger::info("[Client] Shutting down systems...");
    manager.shutdownAll();
    manager.clearAll();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    Logger::shutdown();
    return 0;
}
