/*
** EPITECH PROJECT, 2025
** repo
** File description:
** Packet
*/

#pragma once

#include <vector>
#include <cstring>
#include <stdexcept>
#include "MessageHeader.hpp"

namespace Network {

    struct Packet {
        MessageHeader header;
        std::vector<uint8_t> payload;

        std::vector<uint8_t> serialize() const;
        static Packet deserialize(const uint8_t* data, size_t len);
    };

}