/*
** EPITECH PROJECT, 2025
** Systems
** File description:
** RenderSystem.hpp
*/

#pragma once

#include <raylib.h>
#include <memory>
#include <NetworkContext.hpp>
#include <SpriteManager.hpp>
#include "ISystem.hpp"
#include "Registry.hpp"
#include "Position.hpp"
#include "Velocity.hpp"

class RenderSystem : public ISystem {
    public:
        RenderSystem(int w, int h, const std::string& title, std::shared_ptr<NetworkContext> ctx);
        ~RenderSystem() override;
        void update(Ecs::Registry& registry, float dt) override;
    
    private:
        int _w;
        int _h;
        std::string _title;
        std::shared_ptr<NetworkContext> _ctx;
        SpriteManager _sprites;
};
