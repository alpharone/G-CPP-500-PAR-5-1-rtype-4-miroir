/*
** EPITECH PROJECT, 2025
** repo
** File description:
** MessageHeader
*/

#pragma once

#include <cstdint>

namespace Network {

    #pragma pack(push, 1)

    struct MessageHeader {
        uint8_t type = 0;
        uint16_t length = 0;
        uint32_t seq = 0;
        uint8_t flags = 0;
    } __attribute__((packed));

    #pragma pack(pop)

}