/*
** EPITECH PROJECT, 2025
** Systems
** File description:
** InputSystem.hpp
*/

#pragma once

#include <raylib.h>
#include <unordered_map>
#include <memory>

#include "ISystem.hpp"
#include "NetworkContext.hpp"
#include "Packets.hpp"
#include "Logger.hpp"

namespace System {

    class InputSystem : public ISystem {
        public:
            explicit InputSystem(std::shared_ptr<Network::network_context_t> ctx);
            ~InputSystem() override = default;
        
            void init(Ecs::Registry& registry) override;
            void update(Ecs::Registry& registry, double dt) override;
            void shutdown() override;
        
        private:
            std::shared_ptr<Network::network_context_t> _ctx;
            std::unordered_map<int, uint8_t> _keyMapping;
    };

}