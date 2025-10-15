/*
** EPITECH PROJECT, 2025
** repo
** File description:
** Utils
*/

#pragma once

#include <vector>
#include <cstdint>

namespace Network {

    void write_u8(std::vector<uint8_t>& out, uint8_t v);
    void write_u16_le(std::vector<uint8_t>& out, uint16_t v);
    void write_u32_le(std::vector<uint8_t>& out, uint32_t v);
    void write_u64_le(std::vector<uint8_t>& out, uint64_t v);

    uint8_t read_u8(const uint8_t* data, size_t size, size_t offset);
    uint16_t read_u16_le(const uint8_t* data, size_t size, size_t offset);
    uint32_t read_u32_le(const uint8_t* data, size_t size, size_t offset);
    uint64_t read_u64_le(const uint8_t* data, size_t size, size_t offset);

}
