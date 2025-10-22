/*
** EPITECH PROJECT, 2025
** Network
** File description:
** Utils
*/

#include <stdexcept>
#include "Utils.hpp"

void Network::write_u8(std::vector<uint8_t>& out, uint8_t v)
{
    out.push_back(v);
}

void Network::write_u16_le(std::vector<uint8_t>& out, uint16_t v)
{
    out.push_back(static_cast<uint8_t>(v & 0xFF));
    out.push_back(static_cast<uint8_t>((v >> 8) & 0xFF));
}

void Network::write_u32_le(std::vector<uint8_t>& out, uint32_t v)
{
    out.push_back(static_cast<uint8_t>(v & 0xFF));
    out.push_back(static_cast<uint8_t>((v >> 8) & 0xFF));
    out.push_back(static_cast<uint8_t>((v >> 16) & 0xFF));
    out.push_back(static_cast<uint8_t>((v >> 24) & 0xFF));
}

void Network::write_u32_le(uint8_t* data, uint32_t v)
{
    data[0] = static_cast<uint8_t>(v & 0xFF);
    data[1] = static_cast<uint8_t>((v >> 8) & 0xFF);
    data[2] = static_cast<uint8_t>((v >> 16) & 0xFF);
    data[3] = static_cast<uint8_t>((v >> 24) & 0xFF);
}

void Network::write_u64_le(std::vector<uint8_t>& out, uint64_t v)
{
    for (int i = 0; i < 8; i++) {
        out.push_back(static_cast<uint8_t>((v >> (8 * i)) & 0xFF));
    }
}

uint8_t Network::read_u8(const uint8_t* data, size_t size, size_t offset)
{
    if (offset + 1 > size) {
        throw std::out_of_range("read_u8 out of range");
    }
    return data[offset];
}

uint16_t Network::read_u16_le(const uint8_t* data, size_t size, size_t offset)
{
    if (offset + 2 > size) {
        throw std::out_of_range("read_u16_le out of range");
    }
    return static_cast<uint16_t>(data[offset]) | (static_cast<uint16_t>(data[offset + 1]) << 8);
}

uint32_t Network::read_u32_le(const uint8_t* data, size_t size, size_t offset)
{
    if (offset + 4 > size) {
        throw std::out_of_range("read_u32_le out of range");
    }
    return static_cast<uint32_t>(data[offset])
         | (static_cast<uint32_t>(data[offset + 1]) << 8)
         | (static_cast<uint32_t>(data[offset + 2]) << 16)
         | (static_cast<uint32_t>(data[offset + 3]) << 24);
}

uint64_t Network::read_u64_le(const uint8_t* data, size_t size, size_t offset)
{
    if (offset + 8 > size) {
        throw std::out_of_range("read_u64_le out of range");
    }
    uint64_t v = 0;
    for (int i = 0; i < 8; i++) {
        v |= (static_cast<uint64_t>(data[offset + i]) << (8 * i));
    }
    return v;
}
