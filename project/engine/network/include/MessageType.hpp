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
        NEW_CLIENT = 1,
        ACCEPT_CLIENT,
        CLIENT_INPUT,
        SERVER_SNAPSHOT,
        ENTITY_SPAWN,
        ENTITY_DESPAWN,
        GAME_EVENT,
        ACK,
        PING,
        PONG,
        RELIABLE_FRAGMENT,
        PLAYER_READY,
        GAME_START,
        GAME_OVER,
        ADMIN_TEXT,
        PLAYER_INPUT
    };

}