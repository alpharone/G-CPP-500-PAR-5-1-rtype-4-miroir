/*
** EPITECH PROJECT, 2025
** repo
** File description:
** GameInstance
*/

#pragma once
#include <chrono>
#include <vector>
#include "Registry.hpp"
#include "Packets.hpp"

class GameInstance {
public:
    GameInstance();
    void tick(float dt);
    void applyInput(uint32_t clientId, const std::vector<uint8_t>& input);
    Network::Packet produceSnapshot();
private:
    Ecs::Registry _registry;
};
