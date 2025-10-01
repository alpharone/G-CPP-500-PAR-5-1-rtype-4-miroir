/*
** EPITECH PROJECT, 2025
** repo
** File description:
** MessageType
*/

#pragma once

#include <cstdint>

namespace Network {

    enum MessageType : uint8_t {
        NEW_CLIENT = 0x01,
        ACCEPT_CLIENT = 0x02,
        CLIENT_INPUT = 0x03,
        SERVER_SNAPSHOT = 0x04,
        ENTITY_SPAWN = 0x05,
        ENTITY_DESPAWN = 0x06,
        GAME_EVENT = 0x07,
        ACK = 0x08,
        PING = 0x09,
        PONG = 0x0A,
        RELIABLE_FRAGMENT = 0x0B,
        PLAYER_READY = 0x0C,
        GAME_START = 0x0D,
        GAME_OVER = 0x0E,
        ADMIN_TEXT = 0xFF
    };

}