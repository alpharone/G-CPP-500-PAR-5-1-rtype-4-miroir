/*
** EPITECH PROJECT, 2025
** Client
** File description:
** RenderLoop.hpp
*/

#pragma once

#include <SFML/Graphics.hpp>
#include "ResourceManager.hpp"
#include "Registry.hpp"
#include "Position.hpp"
#include "Drawable.hpp"

class RenderLoop {
    public:
        explicit RenderLoop(ResourceManager& res);
        ~RenderLoop();

        bool initWindow(unsigned int w, unsigned int h, const std::string& title);
        void renderFrame(Ecs::Registry& registry, float dt);
        bool isOpen() const;
        void shutdown();
        sf::RenderWindow& window();

    private:
        ResourceManager& _res;
        sf::RenderWindow _window;

        void renderSprite(const Component::position_t& pos, const Component::drawable_t& dr);
        void renderRectangle(const Component::position_t& pos, const Component::drawable_t& dr);
        void renderEntities(Ecs::Registry& registry);
};
