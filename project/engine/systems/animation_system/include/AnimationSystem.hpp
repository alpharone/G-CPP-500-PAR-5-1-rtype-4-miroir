/*
** EPITECH PROJECT, 2025
** Systems
** File description:
** AnimationSystem.hpp
*/

#pragma once

#include <memory>
#include <string>
#include <any>
#include "ISystem.hpp"
#include "SpriteManager.hpp"

namespace System {

    class AnimationSystem : public ISystem {
        public:
            AnimationSystem();
            ~AnimationSystem() override = default;

            void init(Ecs::Registry& registry) override;
            void update(Ecs::Registry& registry, double dt) override;
            void shutdown() override;

        private:
            std::unique_ptr<SpriteManager> _spriteManager;
    };

}