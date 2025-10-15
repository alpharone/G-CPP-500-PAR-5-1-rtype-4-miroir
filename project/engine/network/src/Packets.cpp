/*
** EPITECH PROJECT, 2025
** repo
** File description:
** Packet
*/

#include "Packets.hpp"
#include "Utils.hpp"

std::vector<uint8_t> Network::Packet::serialize() const
{
    std::vector<uint8_t> out;

    out.reserve(1 + 2 + 4 + payload.size());
    write_u8(out, header.type);
    write_u16_le(out, static_cast<uint16_t>(payload.size()));
    write_u32_le(out, header.seq);
    out.insert(out.end(), payload.begin(), payload.end());
    return out;
}

Network::Packet Network::Packet::deserialize(const uint8_t* data, size_t len)
{
    const size_t HEADER_SIZE = 1 + 2 + 4;

    if (len < HEADER_SIZE)
        throw std::runtime_error("Packet too small for header");
    Packet p;
    size_t offset = 0;

    p.header.type = read_u8(data, len, offset);
    offset += 1;
    uint16_t size = read_u16_le(data, len, offset);
    offset += 2;
    p.header.seq = read_u32_le(data, len, offset);
    offset += 4;
    if (len - offset < size)
        throw std::runtime_error("Packet payload length mismatch");
    p.payload.assign(data + offset, data + offset + size);
    return p;
}
