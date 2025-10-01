/*
** EPITECH PROJECT, 2025
** repo
** File description:
** GameInstance
*/

#include "GameInstance.hpp"
#include "MessageType.hpp"

GameInstance::GameInstance() {}

void GameInstance::tick(float dt)
{
}

void GameInstance::applyInput(uint32_t clientId, const std::vector<uint8_t>& input)
{
}

Network::Packet GameInstance::produceSnapshot() {
    Network::Packet pack;
    pack.header.type = Network::SERVER_SNAPSHOT;
    return pack;
}
