/*
** EPITECH PROJECT, 2025
** Systems
** File description:
** InputSystem.hpp
*/

#pragma once

#include <raylib.h>
#include <asio.hpp>
#include <deque>
#include <cmath>
#include <array>
#include <cstring>
#include "ISystem.hpp"
#include "Registry.hpp"
#include "Logger.hpp"
#include "MessageType.hpp"
#include "Velocity.hpp"
#include "Packets.hpp"
#include "NetworkContext.hpp"

class InputSystem : public ISystem {
    public:
        InputSystem(std::shared_ptr<NetworkContext> ctx);
        ~InputSystem() override = default;

        void update(Ecs::Registry& registry, float dt) override;

    private:
        std::shared_ptr<NetworkContext> _ctx;
};