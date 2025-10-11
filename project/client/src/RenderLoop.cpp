// /*
// ** EPITECH PROJECT, 2025
// ** Client
// ** File description:
// ** RenderLoop.cpp
// */

// #include <iostream>
// #include "RenderLoop.hpp"

// RenderLoop::RenderLoop(ResourceManager& res) : _res(res)
// {}

// RenderLoop::~RenderLoop()
// {
//     shutdown();
// }

// bool RenderLoop::initWindow(unsigned int w, unsigned int h, const std::string& title)
// {
//     _window.create(sf::VideoMode(w, h), title, sf::Style::Close | sf::Style::Titlebar);
//     if (!_window.isOpen()) {
//         std::cerr << "RenderLoop::initWindow - failed to open\n";
//         return false;
//     }
//     _window.setFramerateLimit(60);
//     return true;
// }

// sf::RenderWindow& RenderLoop::window()
// {
//     return _window;
// }

// void RenderLoop::renderSprite(const Component::position_t& pos, const Component::drawable_t& dr)
// {
//     sf::Sprite spr = _res.makeSprite(dr.spriteId);
//     sf::FloatRect bounds = spr.getLocalBounds();
//     if (bounds.width > 0 && bounds.height > 0) {
//         spr.setScale(float(dr.w) / bounds.width, float(dr.h) / bounds.height);
//     }
//     spr.setPosition(pos.x, pos.y);
//     _window.draw(spr);
// }

// void RenderLoop::renderRectangle(const Component::position_t& pos, const Component::drawable_t& dr)
// {
//     sf::RectangleShape rect(sf::Vector2f(static_cast<float>(dr.w), static_cast<float>(dr.h)));
//     rect.setPosition(pos.x - dr.w * 0.5f, pos.y - dr.h * 0.5f);

//     uint32_t c = dr.color;
//     uint8_t r = (c >> 24) & 0xFF;
//     uint8_t g = (c >> 16) & 0xFF;
//     uint8_t bcol = (c >> 8) & 0xFF;
//     uint8_t a = c & 0xFF;
//     rect.setFillColor(sf::Color(r, g, bcol, a));

//     _window.draw(rect);
// }


// void RenderLoop::renderEntities(Ecs::Registry& registry)
// {
//     auto& posArr  = registry.getComponents<Component::position_t>();
//     auto& drawArr = registry.getComponents<Component::drawable_t>();

//     size_t n = std::max(posArr.size(), drawArr.size());
//     for (size_t i = 0; i < n; i++) {
//         const auto& posOpt = posArr[i];
//         const auto& drOpt  = drawArr[i];
//         if (!posOpt || !drOpt)
//             continue;

//         const Component::position_t& pos = *posOpt;
//         const Component::drawable_t& dr  = *drOpt;

//         if (!dr.spriteId.empty() && _res.hasTexture(dr.spriteId)) {
//             renderSprite(pos, dr);
//         } else {
//             renderRectangle(pos, dr);
//         }
//     }
// }

// void RenderLoop::renderFrame(Ecs::Registry& registry, float)
// {
//     _window.clear(sf::Color(10, 10, 30));
//     try {
//         renderEntities(registry);
//     } catch (const std::exception& e) {
//         static bool warned = false;
//         if (!warned) {
//             std::cerr << "RenderLoop::renderFrame: exception: " << e.what() << '\n';
//             warned = true;
//         }
//     }
//     _window.display();
// }

// bool RenderLoop::isOpen() const
// {
//     return _window.isOpen();
// }

// void RenderLoop::shutdown()
// {
//     if (_window.isOpen())
//         _window.close();
// }
