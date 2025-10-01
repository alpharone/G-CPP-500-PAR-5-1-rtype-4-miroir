/*
** EPITECH PROJECT, 2025
** repo
** File description:
** Packet
*/

#include "Packets.hpp"

std::vector<uint8_t> Network::Packet::serialize() const
{
    std::vector<uint8_t> data(sizeof(MessageHeader) + payload.size());
    std::memcpy(data.data(), &header, sizeof(MessageHeader));
    if (!payload.empty()) {
        std::memcpy(data.data() + sizeof(MessageHeader), payload.data(), payload.size());
    }
    return data;
}

Network::Packet Network::Packet::deserialize(const uint8_t* data, size_t len)
{
    Packet p;

    std::memcpy(&p.header, data, sizeof(MessageHeader));
    if (len > sizeof(MessageHeader)) {
        p.payload.resize(len - sizeof(MessageHeader));
        std::memcpy(p.payload.data(), data + sizeof(MessageHeader), p.payload.size());
    }
    return p;
}
