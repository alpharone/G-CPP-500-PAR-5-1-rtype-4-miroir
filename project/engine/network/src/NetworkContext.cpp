/*
** EPITECH PROJECT, 2025
** repo
** File description:
** NetworkContext.cpp
*/

#include "NetworkContext.hpp"

void Network::network_context_t::pushPacket(const Network::Packet& pkt)
{
    std::lock_guard<std::mutex> lock(queueMutex);

    incoming.push(pkt);
}

bool Network::network_context_t::popPacket(Network::Packet& out)
{
    std::lock_guard<std::mutex> lock(queueMutex);

    if (incoming.empty()) {
        return false;
    }
    out = std::move(incoming.front());
    incoming.pop();
    return true;
}
