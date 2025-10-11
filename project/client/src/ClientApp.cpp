// /*
// ** EPITECH PROJECT, 2025
// ** Client
// ** File description:
// ** ClientApp.cpp
// */

// #include <cmath>
// #include <iostream>
// #include <SFML/Window/Keyboard.hpp>
// #include "ClientApp.hpp"
// #include "Position.hpp"
// #include "Drawable.hpp"

// ClientApp::ClientApp() : _render(_res)
// {}

// ClientApp::~ClientApp()
// {
//     shutdown();
// }

// bool ClientApp::init()
// {
//     if (!_render.initWindow(1920, 1080, "R-Type"))
//         return false;

//     _playerEntity = _registry.spawnEntity();
//     _registry.emplaceComponent<Component::position_t>(_playerEntity, 400.f, 300.f);
//     _registry.emplaceComponent<Component::drawable_t>(_playerEntity,
//         static_cast<uint16_t>(40), static_cast<uint16_t>(40), 0xFF0000FFu,
//         std::string(""));

//     auto& posArr  = _registry.getComponents<Component::position_t>();
//     auto& drawArr = _registry.getComponents<Component::drawable_t>();
//     size_t idx = static_cast<size_t>(_playerEntity);

//     if (idx < posArr.size() && posArr[idx] && drawArr[idx]) {
//         auto& pos  = *posArr[idx];
//         auto& draw = *drawArr[idx];
//         (void)pos;
//         (void)draw;
//     } else {
//         std::cerr << "ClientApp::init - failed to access components after emplace\n";
//         return false;
//     }

//     _lastTime = std::chrono::steady_clock::now();
//     return true;
// }

// float ClientApp::computeDeltaTime()
// {
//     auto now = std::chrono::steady_clock::now();
//     std::chrono::duration<float> delta = now - _lastTime;
//     _lastTime = now;
//     return delta.count();
// }

// void ClientApp::handleEvents()
// {
//     sf::Event ev;
//     while (_render.window().pollEvent(ev)) {
//         if (ev.type == sf::Event::Closed) {
//             shutdown();
//             return;
//         }
//     }
// }

// std::pair<float, float> ClientApp::getMovementInput() const
// {
//     float dx = 0.f;
//     float dy = 0.f;

//     if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::A))
//         dx -= 1.f;
//     if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) || sf::Keyboard::isKeyPressed(sf::Keyboard::D))
//         dx += 1.f;
//     if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) || sf::Keyboard::isKeyPressed(sf::Keyboard::W))
//         dy -= 1.f;
//     if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) || sf::Keyboard::isKeyPressed(sf::Keyboard::S))
//         dy += 1.f;

//     if (dx != 0.f || dy != 0.f) {
//         float len = std::sqrt(dx*dx + dy*dy);
//         dx /= len;
//         dy /= len;
//     }

//     return {dx, dy};
// }

// void ClientApp::movePlayer(float dx, float dy, float dt)
// {
//     const float speed = 300.f;

//     try {
//         auto& posArr = _registry.getComponents<Component::position_t>();
//         size_t idx = static_cast<size_t>(_playerEntity);
//         if (idx < posArr.size() && posArr[idx]) {
//             posArr[idx]->x += dx * speed * dt;
//             posArr[idx]->y += dy * speed * dt;
//         }
//     } catch (const std::exception& ex) {
//         static bool warned = false;
//         if (!warned) {
//             std::cerr << "ClientApp::movePlayer failed: " << ex.what() << '\n';
//             warned = true;
//         }
//     }
// }

// void ClientApp::mainLoop()
// {
//     while (_render.isOpen()) {
//         float dt = computeDeltaTime();
//         handleEvents();

//         auto [dx, dy] = getMovementInput();
//         if (dx != 0.f || dy != 0.f) {
//             movePlayer(dx, dy, dt);
//         }

//         _render.renderFrame(_registry, dt);
//     }
// }

// void ClientApp::shutdown()
// {
//     _render.shutdown();
// }
